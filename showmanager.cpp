#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "show.h"
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

void ShowManager::parseSeasons(const QString &url, const QByteArray &response)
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
        query.bindValue(":show_id", url);
        query.bindValue(":number", number);
        query.exec();
        /*        Season *season = getSeasonByNumber(number);
        if (!season) {
            season = new Season(number);
            _seasons << season;
        }*/
        //season->parseEpisodes(seasonJson);
    }

    // update the episodes last check date of the show
    QSqlQuery query;
    query.exec(QString("UPDATE show SET episodes_last_check_date=%1 where id='%2';").arg(QDateTime::currentMSecsSinceEpoch() / 1000).arg(url));
//    QSqlQuery query;
//    query.prepare("UPDATE show SET episodes_last_check_date=:epoch where id=:showid");
//    query.bindValue("epoch", QDateTime::currentMSecsSinceEpoch() / 1000);
//    query.bindValue("showid", "'" + url + "'");
//    query.exec();
}

int ShowManager::refreshOnExpired(const QString &showid, Show::ShowItem item)
{
    QSqlQuery query;
    qint64 last_check_epoch = 0;
    qint64 expiration = 24 * 60 * 60 * 1000;
    int ticket;
    switch (item) {
    case Show::Item_Episodes:
        // have we the season in database?
        // take the expiration date in account
        query.exec(QString("SELECT episodes_last_check_date FROM show WHERE id='%1'").arg(showid));
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
