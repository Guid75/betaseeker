#include "tvshow.h"
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
}

const TvShow &ShowManager::showAt(int index) const
{
    return _shows[index];
}

int ShowManager::indexOfShow(const QString &url) const
{
    for (int i = 0; i < _shows.count(); i++) {
        const TvShow &show = _shows[i];
        if (show.url() == url)
            return i;
    }
    return -1;
}

void ShowManager::addShow(const QString &title, const QString &url)
{
    if (indexOfShow(url) >= 0)
        return;

    TvShow show(title, url);
    _shows << show;
    emit showAdded(show);
}
