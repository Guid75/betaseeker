#include "show.h"
#include "requestmanager.h"
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
    connect(&RequestManager::instance(), SIGNAL(requestFinished(int,QByteArray)),
            this, SLOT(requestFinished(int,QByteArray)));
}

void ShowManager::requestFinished(int ticketId, const QByteArray &response)
{
// TODO
}

const Show &ShowManager::showAt(int index) const
{
    return _shows[index];
}

int ShowManager::indexOfShow(const QString &url) const
{
    for (int i = 0; i < _shows.count(); i++) {
        const Show &show = _shows[i];
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
    switch (item) {
    case Show::Item_Episodes:
        RequestManager::instance().showsEpisodes(show.url());
        break;
    default:
        break;
    }
}

void ShowManager::addShow(const QString &title, const QString &url)
{
    if (indexOfShow(url) >= 0)
        return;

    Show show(title, url);
    _shows << show;
    emit showAdded(show);
}
