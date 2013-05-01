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

#include <QLabel>
#include <QVBoxLayout>
#include <QVariant>
#include <QSqlQuery>
#include <QDesktopServices>
#include <QMovie>

#include "requestmanager.h"
#include "jsonparser.h"
#include "ui_episodewidget.h"
#include "subtitlewidget.h"

#include "episodewidget.h"

EpisodeWidget::EpisodeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EpisodeWidget),
    _state(State_Waiting),
    collapsed(true),
    ticket(-1)
{
    ui->setupUi(this);

    subtitlesLayout = new QVBoxLayout(ui->widgetSubtitles);
    subtitlesLayout->setMargin(0);
    subtitlesLayout->setSpacing(0);

    // TODO connect only at asking
    connect(&RequestManager::instance(), &RequestManager::requestFinished,
            this, &EpisodeWidget::requestFinished);
}

void EpisodeWidget::init(const QSqlRecord &record, const QString &showId, int season)
{
    _record = record;
    _showId = showId;
    _season = season;
    ui->label->setText(tr("Episode %1 (%2)").arg(record.value("episode").toInt()).arg(record.value("title").toString()));
    QIcon icon(":/icons/resources/icon-expand.png");
    QPixmap pix = icon.pixmap(16, 16);
    ui->labelPixmap->setPixmap(pix);
}

int EpisodeWidget::episode() const
{
    return _record.value("episode").toInt();
}

void EpisodeWidget::expand()
{
    if (!collapsed)
        return;

    collapsed = false;
    QIcon icon(":/icons/resources/icon-collapse.png");
    QPixmap pix = icon.pixmap(16, 16);
    ui->labelPixmap->setPixmap(pix);

    if (ui->widgetSubtitles->isHidden() && ticket == -1)
        ui->widgetSubtitles->show();
    else {
        if (ticket == -1)
            ticket = RequestManager::instance().subtitlesShow(_showId, _season, episode());
        QMovie *movie = new QMovie(":/icons/resources/loading.gif");
        movie->setScaledSize(QSize(16, 16));
        ui->labelPixmap->setMovie(movie);
        movie->start();
    }
}

void EpisodeWidget::collapse()
{
    if (collapsed)
        return;

    collapsed = true;
    QIcon icon(":/icons/resources/icon-expand.png");
    QPixmap pix = icon.pixmap(20, 20);
    ui->labelPixmap->setPixmap(pix);
    ui->widgetSubtitles->hide();
}

void EpisodeWidget::toggleCollapse()
{
    if (collapsed)
        expand();
    else
        collapse();
}

bool sortByQuality(SubtitleWidget *w1, SubtitleWidget *w2)
{
    return w1->record().value("quality").toInt() > w2->record().value("quality").toInt();
}

void EpisodeWidget::parseSubtitles(const QByteArray &response)
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
    query.bindValue(":episode", episode());
    query.exec();

    QJsonObject subtitlesJson = parser.root().value("subtitles").toObject();
    if (subtitlesJson.keys().count() == 0)
        return;
    QList<SubtitleWidget*> widgets;
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
        query.prepare("INSERT INTO subtitle (show_id, season, episode, language, "
                      "source, file, url, quality) "
                      "VALUES (:show_id, :season, :episode, "
                      ":language, :source, :file, :url, :quality)");
        query.bindValue(":show_id", _showId);
        query.bindValue(":season", _season);
        query.bindValue(":episode", episode());
        query.bindValue(":language", language);
        query.bindValue(":source", source);
        query.bindValue(":file", file);
        query.bindValue(":url", url);
        query.bindValue(":quality", quality);
        query.exec();

        SubtitleWidget *subtitleWidget = new SubtitleWidget;
        subtitleWidget->installEventFilter(this);
        qint32 rowid = query.lastInsertId().toInt();
        query.exec(QString("SELECT * FROM subtitle where rowid=%1").arg(rowid));
        if (query.next()) {
            subtitleWidget->setRecord(query.record());
        }
        widgets << subtitleWidget;
    }

    qSort(widgets.begin(), widgets.end(), sortByQuality);

    foreach (QWidget *widget, widgets)
        subtitlesLayout->addWidget(widget);

    setState(EpisodeWidget::State_Fetched);

    // update the episodes last check date of the subtitles
//    QSqlQuery query;
//    query.exec(QString("UPDATE show SET episodes_last_check_date=%1 WHERE show_id='%2';").arg(QDateTime::currentMSecsSinceEpoch() / 1000).arg(showId));
}

void EpisodeWidget::requestFinished(int ticketId, const QByteArray &response)
{
    if (ticketId != ticket)
        return;

    ticket = -1;

    QString res;
    if (collapsed)
        res = ":/icons/resources/icon-expand.png";
    else
        res = ":/icons/resources/icon-collapse.png";
    QIcon icon(res);
    QPixmap pix = icon.pixmap(16, 16);
    ui->labelPixmap->setPixmap(pix);

    parseSubtitles(response);
}

bool EpisodeWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        SubtitleWidget *subtitleWidget = qobject_cast<SubtitleWidget*>(obj);
        if (subtitleWidget) {
            QDesktopServices::openUrl(subtitleWidget->record().value("url").toString());
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
