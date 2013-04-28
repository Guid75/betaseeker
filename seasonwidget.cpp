#include <QVBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QVariant>
#include <QEvent>
#include <QMouseEvent>
#include <QDesktopServices>

#include "subtitlewidget.h"
#include "episodewidget.h"
#include "requestmanager.h"
#include "jsonparser.h"

#include "seasonwidget.h"

SeasonWidget::SeasonWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    connect(&RequestManager::instance(), &RequestManager::requestFinished,
            this, &SeasonWidget::requestFinished);
}

void SeasonWidget::init(const QString &showId, int season)
{
    _showId = showId;
    _season = season;

    clear();

    QSqlQuery query;

    query.prepare(QString("SELECT episode FROM episode WHERE show_id='%1' AND season=%2").arg(showId).arg(season));
    query.exec();

    QList<int> eps;
    while (query.next()) {
        eps << query.value("episode").toInt();
    }
    qSort(eps.begin(), eps.end(), qGreater<int>());

    foreach (int ep, eps) {
        query.prepare(QString("SELECT * FROM episode WHERE show_id='%1' AND season=%2 AND episode=%3").arg(showId).arg(season).arg(ep));
        query.exec();

        if (query.next()) {
            EpisodeWidget *widget = new EpisodeWidget;
            widget->init(query.record(), _showId, _season);
            layout()->addWidget(widget);
            widget->installEventFilter(this);
        }
    }
    qobject_cast<QVBoxLayout*>(layout())->addStretch();
}

void SeasonWidget::clear()
{
    // remove all widgets
    QLayoutItem *child;
    while ((child = layout()->takeAt(0)) != 0) {
        if (child->widget())
            delete child->widget();
        delete child;
    }
}

void SeasonWidget::parseSubtitles(EpisodeWidget *episodeWidget, const QByteArray &response)
{
    JsonParser parser(response);
    if (!parser.isValid()) {
        // TODO manage error
        return;
    }

    int episode = episodeWidget->episode();

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

        SubtitleWidget *subtitleWidget = new SubtitleWidget;
        subtitleWidget->installEventFilter(this);
        qint32 rowid = query.lastInsertId().toInt();
        query.exec(QString("SELECT * FROM subtitle where rowid=%1").arg(rowid));
        if (query.next()) {
            subtitleWidget->setRecord(query.record());
        }
        /*episodeNode->children << child;*/
        int epIndex = layout()->indexOf(episodeWidget);
        QVBoxLayout *l = qobject_cast<QVBoxLayout*>(layout());
        l->insertWidget(epIndex + 1, subtitleWidget);
        //layout()->addWidget(subtitleWidget);
        episodeWidget->setState(EpisodeWidget::State_Fetched);
    }

    // update the episodes last check date of the subtitles
//    QSqlQuery query;
//    query.exec(QString("UPDATE show SET episodes_last_check_date=%1 WHERE show_id='%2';").arg(QDateTime::currentMSecsSinceEpoch() / 1000).arg(showId));
}

void SeasonWidget::requestFinished(int ticketId, const QByteArray &response)
{
    EpisodeWidget *episodeWidget = tickets[ticketId];
    if (!episodeWidget)
        return;

    tickets.remove(ticketId);

    parseSubtitles(episodeWidget, response);
}

bool SeasonWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        EpisodeWidget *widget = qobject_cast<EpisodeWidget*>(obj);
        if (widget) {
/*            int ticket = RequestManager::instance().subtitlesShow(_showId, _season, widget->episode());
            tickets.insert(ticket, widget);*/
            widget->toggleCollapse();
            return true;
        }
/*        SubtitleWidget *subtitleWidget = qobject_cast<SubtitleWidget*>(obj);
        if (subtitleWidget) {
            qDebug("click on subtitle");
            QDesktopServices::openUrl(subtitleWidget->record().value("url").toString());
            qDebug(qPrintable(subtitleWidget->record().value("url").toString()));
            return true;
        }*/
    }
    return QObject::eventFilter(obj, event);
}
