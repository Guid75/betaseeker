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

#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

#include "requestmanager.h"
#include "jsonparser.h"
#include "showmanager.h"

ShowManager *ShowManager::_instance = 0;

ShowManager &ShowManager::instance()
{
	if (!_instance)
		_instance = new ShowManager();
	return *_instance;
}

ShowManager::ShowManager() :
	QObject()
{
    connect(&RequestManager::instance(), &RequestManager::requestFinished,
			this, &ShowManager::requestFinished);
}

void ShowManager::requestFinished(int ticketId, const QByteArray &response)
{
    TicketData ticketData = parsing[ticketId];
    if (ticketData.url.isNull())
		return;

	parsing.remove(ticketId);

	// call the parse function
    QMetaObject::invokeMethod(this, ticketData.parseMethodName.toLocal8Bit(), Q_ARG(QString, ticketData.url), Q_ARG(QByteArray, response));

    emit refreshDone(ticketData.url, ticketData.showItem);
}

void ShowManager::parseEpisode(const QString &showId, int season, const QJsonObject &root)
{
    bool ok;
    int episode = root.value("episode").toString().toInt(&ok);
    if (!ok)
        return;
    QString title = root.value("title").toString();
    QString number = root.value("number").toString();
    int global = root.value("global").toString().toInt();
    int date = root.value("date").toDouble();
    int comments = root.value("comments").toString().toInt();

    QSqlQuery query;
    query.prepare("INSERT INTO episode (show_id, season, episode, title, number, global, date, comments) "
                  "VALUES (:show_id, :season, :episode, :title, :number, :global, :date, :comments)");
    query.bindValue(":show_id", showId);
    query.bindValue(":season", season);
    query.bindValue(":episode", episode);
    query.bindValue(":title", title);
    query.bindValue(":number", number);
    query.bindValue(":global", global);
    query.bindValue(":date", date);
    query.bindValue(":comments", comments);
    query.exec();
}

void ShowManager::parseEpisodes(const QString &showId, int season, const QJsonObject &root)
{
    QJsonObject episodesJson = root.value("episodes").toObject();
    foreach (const QString &key, episodesJson.keys()) {
        QJsonObject episodeJson = episodesJson.value(key).toObject();
        if (episodeJson.isEmpty())
            continue;
        parseEpisode(showId, season, episodeJson);
    }
}

void ShowManager::parseSeasons(const QString &showId, const QByteArray &response)
{
	JsonParser parser(response);
    if (!parser.isValid()) {
		// TODO manage error
		return;
	}

    QJsonObject seasonsJson = parser.root().value("seasons").toObject();
    foreach (const QString &key, seasonsJson.keys()) {
        QJsonObject seasonJson = seasonsJson.value(key).toObject();
        if (seasonJson.isEmpty())
            continue;

        // numbers are under the form of strings from betaseries :/
        bool ok;
        int number = seasonJson.value("number").toString().toInt(&ok);
        if (!ok)
            continue;

        QSqlQuery query;
        query.prepare("INSERT INTO season (show_id, number) "
                      "VALUES (:show_id, :number)");
        query.bindValue(":show_id", showId);
        query.bindValue(":number", number);
        query.exec();

        parseEpisodes(showId, number, seasonJson);
    }

    // update the episodes last check date of the show
    QSqlQuery query;
    query.prepare("UPDATE show SET episodes_last_check_date=:epoch WHERE show_id=:showid");
    query.bindValue(":epoch", QDateTime::currentMSecsSinceEpoch() / 1000);
    query.bindValue(":showid", showId);
    query.exec();
}

int ShowManager::refreshOnExpired(const QString &showid, Item item)
{
    QSqlQuery query;
    qint64 last_check_epoch = 0;
    qint64 expiration = 24 * 60 * 60 * 1000; // one day => TODO parametrable
    int ticket;
    switch (item) {
    case Item_Episodes:
        // have we the season in database?
        // take the expiration date in account
        query.exec(QString("SELECT episodes_last_check_date FROM show WHERE show_id='%1'").arg(showid));
        if (query.next() && !query.value(0).isNull()) {
            last_check_epoch = query.value(0).toLongLong() * 1000;
        }
        if (QDateTime::currentDateTime().toMSecsSinceEpoch() - last_check_epoch > expiration) {
            // expired data, we need to launch the request
            ticket = RequestManager::instance().showsEpisodes(showid);
            // TODO ticket can be invalid, manage it
            parsing.insert(ticket, TicketData(showid, "parseSeasons", Item_Episodes));
            return 1;
        } else
            return 0;
        break;
    default:
        break;
    }
    return -1;
}
