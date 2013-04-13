#ifndef SHOWMANAGER_H
#define SHOWMANAGER_H

#include <QObject>

#include "show.h"

class ShowManager : public QObject
{
    Q_OBJECT
public:
    static ShowManager &instance();

    int showsCount() const { return _shows.count(); }
    const Show &showAt(int index) const;
    void addShow(const QString &title, const QString &url);
    int indexOfShow(const QString &url) const;

signals:
    void showAdded(const Show &show);

public slots:
    
private:
    static ShowManager *_instance;
    QList<Show> _shows;

    explicit ShowManager();

};

#endif // SHOWMANAGER_H
