#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFile>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager &instance();

public:
    bool openDB();
    QSqlError lastError();

private:
    static DatabaseManager *_instance;
    QSqlDatabase db;

    explicit DatabaseManager(QObject *parent = 0);
};

#endif // DATABASEMANAGER_H
