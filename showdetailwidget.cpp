//  Copyright 2013 Guillaume Denry (guillaume.denry@gmail.com)
//  This file is part of BetaSeeker.
//
//  BetaSeeker is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  BetaSeeker is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with BetaSeeker.  If not, see <http://www.gnu.org/licenses/>.

#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDesktopServices>
#include <QDateTime>
#include <QUrl>
#include <QRegularExpression>

#include "settings.h"
#include "episodemodel.h"
#include "jsonparser.h"
#include "commandmanager.h"
#include "downloadmanager.h"
#include "loadingwidget.h"
#include "subtitlemodel.h"
#include "linkdelegate.h"

#include "ui_showdetailwidget.h"
#include "showdetailwidget.h"

ShowDetailWidget::ShowDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDetailWidget)
{
    ui->setupUi(this);

    episodeModel = new QSqlTableModel(this, QSqlDatabase::database());
    episodeModel->setTable("episode");
    episodeModel->setEditStrategy(QSqlTableModel::OnFieldChange); // probably not necessary since we don't modify table here

    episodeProxyModel = new EpisodeModel(this);
    episodeProxyModel->setSourceModel(episodeModel);

    ui->listViewEpisodes->setModel(episodeProxyModel);
    ui->listViewEpisodes->setModelColumn(0);
    ui->listViewEpisodes->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->listViewEpisodes->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ShowDetailWidget::currentEpisodeChanged);

    subtitleModel = new QSqlTableModel(this, QSqlDatabase::database());
    subtitleModel->setTable("subtitle");
    subtitleModel->setSort(subtitleModel->fieldIndex("quality"), Qt::DescendingOrder);
    subtitleModel->setEditStrategy(QSqlTableModel::OnFieldChange); // probably not necessary since we don't modify table here

    SubtitleModel *subtitleProxyModel = new SubtitleModel(this);
    subtitleProxyModel->setSourceModel(subtitleModel);

    ui->listViewSubtitles->setModel(subtitleProxyModel);
    ui->listViewSubtitles->setModelColumn(0);
    ui->listViewSubtitles->setSelectionBehavior(QAbstractItemView::SelectRows);
    LinkDelegate *linkDelegate = new LinkDelegate;
    connect(linkDelegate, &LinkDelegate::linkClicked, this, &ShowDetailWidget::linkClicked);
    ui->listViewSubtitles->setItemDelegate(linkDelegate);
    ui->listViewSubtitles->installEventFilter(this);

    connect(&CommandManager::instance(), &CommandManager::commandFinished,
            this, &ShowDetailWidget::commandFinished);
}

void ShowDetailWidget::init(const QString &showId, int season)
{
    _showId = showId;
	_season = season;

    episodeModel->setSort(0, Qt::DescendingOrder);
    episodeModel->setFilter(QString("show_id='%1' AND season=%2").arg(_showId).arg(_season));
    episodeModel->select();

    ui->widgetSeasonDir->setVisible(Settings::directoryForSeason(_showId, _season).isEmpty());
}

void ShowDetailWidget::on_pushButtonDefineIt_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, tr("Pick your show season directory"));
    if (filePath.isEmpty())
        return;
    Settings::setDirectoryForSeason(_showId, _season, filePath);
    ui->widgetSeasonDir->hide();
}

void ShowDetailWidget::on_pushButtonForgetIt_clicked()
{
    ui->widgetSeasonDir->hide();
}

void ShowDetailWidget::on_toolButtonRefreshSubtitles_clicked()
{
    QModelIndex current = ui->listViewEpisodes->currentIndex();
    if (!current.isValid())
        return;

    QSqlRecord record = episodeModel->record(current.row());
    int episode = record.value("episode").toInt();

    // invalidate database last check data and relaunch the request
    if (tickets.key(episode, -1) != -1)
        return;

    int ticket = CommandManager::instance().subtitlesShow(_showId, _season, episode);
    tickets.insert(ticket, episode);
    LoadingWidget::showLoadingMask(ui->listViewSubtitles);
}

void ShowDetailWidget::currentEpisodeChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.count() == 0)
        return;

    QSqlRecord record = episodeModel->record(selected.indexes()[0].row());
    int episode = record.value("episode").toInt();

    subtitleModel->setFilter(QString("show_id='%1' AND season=%2 AND episode=%3").arg(_showId).arg(_season).arg(episode));

    QSqlQuery query;
    qint64 last_check_epoch = 0;
    qint64 expiration = 24 * 60 * 60 * 1000; // one day => TODO parametrable

    query.prepare("SELECT subtitles_last_check_date FROM episode WHERE show_id=:show_id AND season=:season AND episode=:episode");
    query.bindValue(":show_id", _showId);
    query.bindValue(":season", _season);
    query.bindValue(":episode", episode);
    query.exec();
    if (query.next() && !query.value(0).isNull()) {
        last_check_epoch = query.value(0).toLongLong() * 1000;
    }
    if (QDateTime::currentDateTime().toMSecsSinceEpoch() - last_check_epoch > expiration) {
        // expired data, we need to launch the request
        int ticket = CommandManager::instance().subtitlesShow(_showId, _season, episode);
        tickets.insert(ticket, episode);
        LoadingWidget::showLoadingMask(ui->listViewSubtitles);
    } else {
        subtitleModel->select();
    }
}

void ShowDetailWidget::parseSubtitles(int episode, const QByteArray &response)
{
    JsonParser parser(response);
    if (!parser.isValid()) {
        // TODO manage error
        return;
    }

    // remove all subtitles for an episode
    QSqlQuery query;
    query.prepare("DELETE FROM subtitle WHERE show_id=:show_id AND season=:season AND episode=:episode");
    query.bindValue(":show_id", _showId);
    query.bindValue(":season", _season);
    query.bindValue(":episode", episode);
    query.exec();

    QJsonObject subtitlesJson = parser.root().value("subtitles").toObject();
    if (subtitlesJson.keys().count() == 0)
        return;
    foreach (const QString &key, subtitlesJson.keys()) {
        QJsonObject subtitleJson = subtitlesJson.value(key).toObject();
        if (subtitleJson.isEmpty())
            continue;

        QString language = subtitleJson.value("language").toString();
        QString source = subtitleJson.value("source").toString();
        QString file = subtitleJson.value("file").toString();
        QString url = subtitleJson.value("url").toString();
        int quality = subtitleJson.value("quality").toDouble();

        QSqlQuery query;
        // TODO smash data if already exists
        query.prepare("INSERT INTO subtitle (show_id, season, episode, language, "
                      "source, file, url, quality) "
                      "VALUES (:show_id, :season, :episode, "
                      ":language, :source, :file, :url, :quality)");
        query.bindValue(":show_id", _showId);
        query.bindValue(":season", _season);
        query.bindValue(":episode", episode);
        query.bindValue(":language", language);
        query.bindValue(":source", source);
        query.bindValue(":file", file);
        query.bindValue(":url", url);
        query.bindValue(":quality", quality);
        query.exec();
    }

    subtitleModel->select();

    // update the episodes last check date of the subtitles
    query.prepare("UPDATE episode SET subtitles_last_check_date=:date WHERE show_id=:show_id AND season=:season AND episode=:episode");
    query.bindValue(":date", QDateTime::currentMSecsSinceEpoch() / 1000);
    query.bindValue(":show_id", _showId);
    query.bindValue(":season", _season);
    query.bindValue(":episode", episode);
    query.exec();
}

bool ShowDetailWidget::eventFilter(QObject *watched, QEvent *event)
{
    // Really UGLY hack, it would be better to delegate this event to the LinkDelegate :/
    if (watched == ui->listViewSubtitles && event->type() == QEvent::Leave) {
        if (QApplication::overrideCursor() && QApplication::overrideCursor()->shape() == Qt::PointingHandCursor)
            QApplication::restoreOverrideCursor();
    }
    return QWidget::eventFilter(watched, event);
}

void ShowDetailWidget::commandFinished(int ticketId, const QByteArray &response)
{
    if (tickets.find(ticketId) == tickets.end())
        return;

    LoadingWidget::hideLoadingMask(ui->listViewSubtitles);

    int episode = tickets[ticketId];
    tickets.remove(ticketId);
    parseSubtitles(episode, response);
}

void ShowDetailWidget::downloadFinished(int ticketId)
{
    // TODO notice the end of download to the user

}

void ShowDetailWidget::linkClicked(const QModelIndex &index)
{
    QString url = subtitleModel->record(index.row()).value("url").toString();
    url.replace(QRegularExpression("^https:"), "http:");
    qDebug(qPrintable(url));
    QString dir = Settings::directoryForSeason(_showId, _season);
    if (dir.isEmpty())
        dir = QDir::tempPath();
    DownloadManager::instance().download(subtitleModel->record(index.row()).value("file").toString(),
                                         url,
                                         dir);
}
