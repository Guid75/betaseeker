#ifndef SHOWMANAGER_H
#define SHOWMANAGER_H

#include <QObject>
#include <QHash>

#include "show.h"

class ShowManager : public QObject
{
	Q_OBJECT
public:
    static ShowManager &instance();

	int showsCount() const { return _shows.count(); }
	const Show &showAt(int index) const;
	Show &showAt(int index);
	void addShow(const QString &title, const QString &url);
	int indexOfShow(const QString &url) const;

    // Load a show item from the database or directly from the website if lifetime is exceed
    void load(const QString &showid, Show::ShowItem item);

    // deprecated
	void refresh(const QString &url, Show::ShowItem item);

    void populateFromDB();

signals:
	void showAdded(const Show &show);
    void refreshDone(const QString &url, Show::ShowItem item);

private:
    struct TicketData {
        TicketData() {}
        TicketData(const QString &_url, const QString &_parseMethodName, Show::ShowItem _showItem) :
            url(_url), parseMethodName(_parseMethodName), showItem(_showItem) {}
        QString url;
        QString parseMethodName;
        Show::ShowItem showItem;
    };

	static ShowManager *_instance;
	QList<Show*> _shows;
    QHash<int,TicketData> parsing;

    explicit ShowManager();
	Show *showAt(const QString &url);

private slots:
	void requestFinished(int ticketId, const QByteArray &response);

    // parsing methods
    void parseEpisodes(const QString &url, const QByteArray &response);
};

#endif // SHOWMANAGER_H
