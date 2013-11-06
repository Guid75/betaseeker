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
#include <QStandardItemModel>

#include <quazip.h>
#include <quazipfile.h>

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

namespace {
    const int subtitleZipNode         = 1;
    const int subtitleZipChildNode    = 2;
    const int subtitleMergedChildNode = 3;
}

ShowDetailWidget::ShowDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDetailWidget),
    downloadTicket(-1)
{

    ui->setupUi(this);

    episodeModel = new QSqlTableModel(this, QSqlDatabase::database());
    episodeModel->setTable("episode");
    episodeModel->setEditStrategy(QSqlTableModel::OnFieldChange); // probably not necessary since we don't modify table here

	episodeProxyModel = new SeasonListModel(this);
    episodeProxyModel->setSourceModel(episodeModel);

    ui->listViewEpisodes->setModel(episodeProxyModel);
    ui->listViewEpisodes->setModelColumn(0);
    ui->listViewEpisodes->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->listViewEpisodes->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ShowDetailWidget::currentEpisodeChanged);

    /*	subtitleModel = new QSqlTableModel(this, QSqlDatabase::database());
    subtitleModel->setTable("subtitle");
    subtitleModel->setSort(subtitleModel->fieldIndex("quality"), Qt::DescendingOrder);
    subtitleModel->setEditStrategy(QSqlTableModel::OnFieldChange); // probably not necessary since we don't modify table here

    SubtitleModel *subtitleProxyModel = new SubtitleModel(this);
    subtitleProxyModel->setSourceModel(subtitleModel);

    ui->listViewSubtitles->setModel(subtitleProxyModel);
    ui->listViewSubtitles->setModelColumn(0);
    ui->listViewSubtitles->setSelectionBehavior(QAbstractItemView::SelectRows);*/

    subtitleModel = new QStandardItemModel(this);
    ui->treeViewSubtitles->setModel(subtitleModel);

    LinkDelegate *linkDelegate = new LinkDelegate;
    connect(linkDelegate, &LinkDelegate::linkClicked, this, &ShowDetailWidget::linkClicked);
    ui->treeViewSubtitles->setItemDelegate(linkDelegate);
    ui->treeViewSubtitles->installEventFilter(this);

    connect(&CommandManager::instance(), &CommandManager::commandFinished,
            this, &ShowDetailWidget::commandFinished);
    connect(&DownloadManager::instance(), &DownloadManager::downloadFinished,
            this, &ShowDetailWidget::downloadFinished);
}

void ShowDetailWidget::init(const QString &showId, int season)
{
    _showId = showId;
    _season = season;

    episodeModel->setSort(episodeModel->fieldIndex("episode"), Qt::DescendingOrder);
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
    LoadingWidget::showLoadingMask(ui->treeViewSubtitles);
}

void ShowDetailWidget::currentEpisodeChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.count() == 0)
        return;

    QSqlRecord record = episodeModel->record(selected.indexes()[0].row());
    int episode = record.value("episode").toInt();

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
        LoadingWidget::showLoadingMask(ui->treeViewSubtitles);
    } else {
        refreshSubtitleTree(episode);
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
    foreach (const QString &key, subtitlesJson.keys()) {
        QJsonObject subtitleJson = subtitlesJson.value(key).toObject();
        if (subtitleJson.isEmpty())
            continue;

        QString language = subtitleJson.value("language").toString();
        QString source = subtitleJson.value("source").toString();
        QString file = subtitleJson.value("file").toString();
        QString url = subtitleJson.value("url").toString();
        int quality = subtitleJson.value("quality").toDouble();

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
        if (!query.exec()) {
            qCritical("Insertion failed, the subtitle parsing is stopped");
            break;
        }

        // content?
        int subtitleId = query.lastInsertId().toInt();

        QJsonObject contentJson = subtitleJson.value("content").toObject();
        if (!contentJson.isEmpty()) {
            // use a QMap to keep all strings ordered as sent by the webserver
            QMap<int,QString> files;
            foreach (const QString &key, contentJson.keys()) {
                QString v = contentJson.value(key).toString();
                if (!v.isEmpty())
                    files.insert(key.toInt(), v);
            }

            QMapIterator<int,QString> i(files);
            while (i.hasNext()) {
                i.next();
                query.prepare("INSERT INTO subtitle_content (subtitle_id, file) "
                              "VALUES (:subtitle_id, :file)");
                query.bindValue(":subtitle_id", subtitleId);
                query.bindValue(":file", i.value());
                query.exec();
            }
        }
    }

    refreshSubtitleTree(episode);

    //	subtitleModel->select();

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
    if (watched == ui->treeViewSubtitles && event->type() == QEvent::Leave) {
        if (QApplication::overrideCursor() && QApplication::overrideCursor()->shape() == Qt::PointingHandCursor)
            QApplication::restoreOverrideCursor();
    }
    return QWidget::eventFilter(watched, event);
}

void ShowDetailWidget::refreshSubtitleTree(int episode)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM episode WHERE show_id=:show_id AND season=:season AND episode=:episode");
    query.bindValue(":show_id", _showId);
    query.bindValue(":season", _season);
    query.bindValue(":episode", episode);
    if (!query.exec()) {
        qCritical("episode fetch failed!");
        return;
    }

    QSqlRecord epRecord;

    if (query.next())
        epRecord = query.record();

    query.prepare("SELECT * FROM subtitle WHERE show_id=:show_id AND season=:season AND episode=:episode");
    query.bindValue(":show_id", _showId);
    query.bindValue(":season", _season);
    query.bindValue(":episode", episode);
    if (!query.exec()) {
        qCritical("subtitles fetch failed!");
        return;
    }

    QStandardItem *parentItem = subtitleModel->invisibleRootItem();
    parentItem->removeRows(0, parentItem->rowCount());

    // TODO : no need to display this node if no movie is selected
    QStandardItem *automaticRoot = new QStandardItem(tr("Subtitles matching the selected movie filename"));
    automaticRoot->setEditable(false);
    parentItem->appendRow(automaticRoot);

    QStandardItem *allRoot = new QStandardItem(tr("All subtitles for Episode %1 (%2)").arg(epRecord.value("episode").toInt()).arg(epRecord.value("title").toString()));
    allRoot->setEditable(false);
    parentItem->appendRow(allRoot);

    QMap<QString,QStandardItem*> langNodes;

    while (query.next()) {
        QString lang = query.value("language").toString();
        QStandardItem *langNode = langNodes[lang];
        if (!langNode) {
            langNode = new QStandardItem(lang);
            langNodes.insert(lang, langNode);
        }

        QSqlQuery subtitleQuery;
        subtitleQuery.prepare("SELECT * from subtitle_content WHERE subtitle_id=:subtitle_id");
        subtitleQuery.bindValue(":subtitle_id", query.value("id"));
        subtitleQuery.exec();
        QList<QStandardItem*> children;
        QStandardItem *contentItem;
        while (subtitleQuery.next()) {
            contentItem = new QStandardItem(subtitleQuery.value("file").toString());
            contentItem->setData(subtitleZipChildNode);
            contentItem->setData(subtitleQuery.value("file"), Qt::UserRole + 2);
            contentItem->setEditable(false);
            children << contentItem;
        }

        if (children.count() == 1) {
            // only one child? no need to make subnodes, this only child become the content root node
            contentItem = children[0];
            contentItem->setText(tr("%1 [zipped]").arg(contentItem->data(Qt::UserRole + 2).toString()));
            contentItem->setData(subtitleMergedChildNode);
            contentItem->setData(query.value("url"), Qt::UserRole + 3);
            contentItem->setData(query.value("file"), Qt::UserRole + 4);
            langNode->appendRow(contentItem);
        } else {
            QString caption;
            if (children.count() > 0)
                caption = tr("%1 (%2 files)").arg(query.value("file").toString()).arg(children.count());
            else
                caption = QString("%1").arg(query.value("file").toString()).arg(query.value("language").toString());
            QStandardItem *subtitleItem = new QStandardItem(caption);
            subtitleItem->setData(subtitleZipNode);
            subtitleItem->setData(query.value("file"), Qt::UserRole + 2);
            subtitleItem->setData(query.value("url"), Qt::UserRole + 3);
            subtitleItem->setEditable(false);
            langNode->appendRow(subtitleItem);
            foreach (QStandardItem *contentItem, children) {
                subtitleItem->appendRow(contentItem);
            }
        }
    }

    // insert lang nodes and update titles
    QMap<QString, QStandardItem*>::const_iterator i = langNodes.constBegin();
    while (i != langNodes.constEnd()) {
        QStandardItem *node = i.value();
        allRoot->appendRow(node);
        ui->treeViewSubtitles->expand(node->index());
        node->setText(tr("%1 (%2 files)").arg(i.key()).arg(node->rowCount()));
        ++i;
    }

    ui->treeViewSubtitles->expand(automaticRoot->index());
    ui->treeViewSubtitles->expand(allRoot->index());
}

void ShowDetailWidget::commandFinished(int ticketId, const QByteArray &response)
{
    if (tickets.find(ticketId) == tickets.end())
        return;

    LoadingWidget::hideLoadingMask(ui->treeViewSubtitles);

    int episode = tickets[ticketId];
    tickets.remove(ticketId);
    parseSubtitles(episode, response);
}

void ShowDetailWidget::downloadFinished(int ticketId, const QString &filePath, const QVariant &userData)
{
    if (ticketId != downloadTicket)
        return;

    downloadTicket = -1;

    QFileInfo fileInfo(filePath);
    if (fileInfo.suffix().compare("zip", Qt::CaseInsensitive) != 0) {
        // not a zip
        return;
    }

    // TODO notice the end of download to the user
    QuaZip quazip(QDir::toNativeSeparators(filePath));
    if (!quazip.open(QuaZip::mdUnzip)) {
        qDebug("Cannot open the zip archive");
        return;
    }

    QString onlyFilePath = userData.toString();

    QuaZipFile zipFile(&quazip);

    QDir dir = QFileInfo(filePath).absoluteDir();

    for (bool f = quazip.goToFirstFile(); f; f = quazip.goToNextFile()) {
        if (!onlyFilePath.isEmpty() && quazip.getCurrentFileName() != onlyFilePath)
            continue;

        QFile outFile(dir.filePath(quazip.getCurrentFileName()));
        outFile.open(QFile::WriteOnly);

        if (!zipFile.open(QIODevice::ReadOnly)) {
            qCritical("Cannot open the file inside the zip");
            continue;
        }

        QByteArray bArray = zipFile.readAll();

        outFile.write(bArray);
        outFile.close();

        zipFile.close();
    }

    quazip.close();

    // we don't need the zip file anymore
    // TODO: improvment: zip must stay persistent until the program extinction
    QFile file(filePath);
    if (!file.remove()) {
        qCritical("Cannot remove %s", qPrintable(filePath));
        return;
    }
}

void ShowDetailWidget::linkClicked(const QModelIndex &index)
{
    // is there a reception directory for those subtitle(s)?
    QString dir = Settings::directoryForSeason(_showId, _season);
    if (dir.isEmpty())
        on_pushButtonDefineIt_clicked();

    QStandardItem *item = subtitleModel->itemFromIndex(index);

    QVariant userData;
    QString file, url;
    if (index.data(Qt::UserRole + 1).toInt() == subtitleZipChildNode) {
        userData = QVariant(item->data(Qt::UserRole + 2).toString());
        item = item->parent();
        file = item->data(Qt::UserRole + 2).toString();
        url = item->data(Qt::UserRole + 3).toString();
    } else if (index.data(Qt::UserRole + 1).toInt() == subtitleMergedChildNode) {
        userData = QVariant(item->data(Qt::UserRole + 2).toString());
        file = item->data(Qt::UserRole + 4).toString();
        url = item->data(Qt::UserRole + 3).toString();
    } else {
        file = item->data(Qt::UserRole + 2).toString();
        url = item->data(Qt::UserRole + 3).toString();
    }

    url.replace(QRegularExpression("^https:"), "http:");
    dir = Settings::directoryForSeason(_showId, _season);
    if (dir.isEmpty())
        dir = QDir::tempPath();
    downloadTicket = DownloadManager::instance().download(file, url, dir, userData);
}
