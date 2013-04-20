#ifndef SHOWMANAGER_H
#define SHOWMANAGER_H

#include <QObject>
#include <QHash>

#include "show.h"

class ShowManager : public QObject
{
	Q_OBJECT
public:
    enum Item {
        Item_Episodes,
        Item_Subtitles
    };

    static ShowManager &instance();

    /*! \brief If a show item is expired, reload it from the website
     * \retval 0 if the show item is still fresh
     * \retval 1 if the show is expired and a request ticket has been emitted
     * \retval -1 if the request ticket could not be given by the request manager
     */
    int refreshOnExpired(const QString &showid, Show::ShowItem item);

signals:
	void showAdded(const Show &show);
    void refreshDone(const QString &url, Item item);

private:
    struct TicketData {
        TicketData() {}
        TicketData(const QString &_url, const QString &_parseMethodName, Item _showItem) :
            url(_url), parseMethodName(_parseMethodName), showItem(_showItem) {}
        QString url;
        QString parseMethodName;
        Item showItem;
    };

	static ShowManager *_instance;
    QHash<int,TicketData> parsing;

    explicit ShowManager();

private slots:
	void requestFinished(int ticketId, const QByteArray &response);

    // parsing methods
    void parseEpisode(const QString &showId, int season, const QJsonObject &root);
    void parseEpisodes(const QString &showId, int season, const QJsonObject &root);
    void parseSeasons(const QString &showId, const QByteArray &response);
};

#endif // SHOWMANAGER_H
