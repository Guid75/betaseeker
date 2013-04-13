#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>

class RequestManager : public QObject
{
    Q_OBJECT
public:
    static RequestManager &instance();

signals:
    
public slots:

private:
    static RequestManager *_instance;
    explicit RequestManager(QObject *parent = 0);

};

#endif // REQUESTMANAGER_H
