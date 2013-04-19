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
	/// \brief openDB try to open the persistent sqlite database and if it fails, it opens a memory database
	/// \return 0 if none of those methods worked
	/// \return 1 if the fallback memory database has been successfully opened
	/// \return 2 if the persistent database has been successfully opened
	int openDB();

	QSqlError lastError();

private:
	static DatabaseManager *_instance;
	QSqlDatabase db;

	explicit DatabaseManager(QObject *parent = 0);
};

#endif // DATABASEMANAGER_H
