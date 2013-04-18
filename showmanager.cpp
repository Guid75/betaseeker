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

void ShowManager::populateFromDB()
{
    //QSqlQuery query("SELECT name FROM sqlite_master WHERE type='table' AND name='shows'");
    //QSqlQuery query("create table episodes(one varchar(10), two smallint)");

//    query.prepare("create table tbl3(one varchar(10), two smallint)");
//    query.prepare("INSERT INTO tbl1 VALUES ('lu', 20)");
//    query.prepare("SELECT * FROM tbl1");
//    if (query.exec())
//        qDebug("success");
//    else
//        qDebug("failure");
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

void ShowManager::parseEpisodes(const QString &url, const QByteArray &response)
{
	Show *show = instance().showAt(url);
	JsonParser parser(response);

    if (!parser.isValid()) {
		// TODO manage error
		return;
	}

    show->parseEpisodes(parser.root());
}

const Show &ShowManager::showAt(int index) const
{
	return *_shows[index];
}

Show &ShowManager::showAt(int index)
{
	return *_shows[index];
}

Show *ShowManager::showAt(const QString &url)
{
	int index = indexOfShow(url);
	if (index >= 0)
		return _shows[index];
	return 0;
}

int ShowManager::indexOfShow(const QString &url) const
{
	for (int i = 0; i < _shows.count(); i++) {
		const Show &show = *_shows[i];
		if (show.url() == url)
			return i;
	}
	return -1;
}

void ShowManager::refresh(const QString &url, Show::ShowItem item)
{
	int index = indexOfShow(url);
	if (index < 0)
		return;
	const Show &show = showAt(index);
	int ticket;
	switch (item) {
	case Show::Item_Episodes:
		ticket = RequestManager::instance().showsEpisodes(show.url());
        parsing.insert(ticket, TicketData(show.url(), "parseEpisodes", Show::Item_Episodes));
		break;
	default:
		break;
	}
}

void ShowManager::addShow(const QString &title, const QString &url)
{
	if (indexOfShow(url) >= 0)
		return;

	Show *show = new Show(title, url);
	_shows << show;
	emit showAdded(*show);
}
