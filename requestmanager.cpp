#include "requestmanager.h"

RequestManager *RequestManager::_instance = 0;

RequestManager::RequestManager(QObject *parent) :
    QObject(parent)
{
}


RequestManager &RequestManager::instance()
{
    if (!_instance)
        _instance = new RequestManager();
    return *_instance;
}
