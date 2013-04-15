#include <QStringList>

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
	QPair<QString,parse_func> parse = parsing[ticketId];
	if (parse.first.isNull())
		return;

	parsing.remove(ticketId);

	// call the parse function
	parse.second(parse.first, response);
}

void ShowManager::parseEpisodes(const QString &url, const QByteArray &response)
{
	Show *show = instance().showAt(url);
	JsonParser parser(response);

	if (!parser.isValid()) {
		// TODO manage error
		return;
	}

	QJsonObject seasons = parser.root().value("seasons").toObject();
	if (seasons.isEmpty()) {
		// TODO manage error
		return;
	}

	foreach (const QString &key, seasons.keys()) {
		QJsonObject season = seasons.value(key).toObject();
		if (season.isEmpty()) {
			// TODO manage error
			continue;
		}

		int number = season.value("number").toDouble(-1);
		if (number == -1) {
			// TODO manage error
			continue;
		}


	}
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
		parsing.insert(ticket, QPair<QString,parse_func>(show.url(), &ShowManager::parseEpisodes));
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
