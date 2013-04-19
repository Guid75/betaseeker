#include <QDir>
#include <QSqlQuery>

#include "databasemanager.h"

DatabaseManager *DatabaseManager::_instance = 0;

DatabaseManager::DatabaseManager(QObject *parent) :
	QObject(parent)
{
}

int DatabaseManager::openDB()
{
	// Find QSLite driver
	db = QSqlDatabase::addDatabase("QSQLITE");

	// NOTE: We have to store database file into user home folder in Linux
	QString path(QDir::home().path());
	path.append(QDir::separator()).append(".betaseeker");
	if (!QDir(path).exists())
		QDir(QDir::home().path()).mkdir(".betaseeker");
	path.append(QDir::separator()).append("shows.sqlite");
	path = QDir::toNativeSeparators(path);
	db.setDatabaseName(path);

	int ret = 2;
	// Open database
	if (!db.open()) {
		ret = 1;
		// now let try with a memory database
		db.setDatabaseName(":memory:");
		if (!db.open()) {
			// even the memory fallback failed? Ouch!
			return 0;
		}
	}

	// create tables?
	// TODO: do not create if they exist and if they embed the needed fields
	QSqlQuery query;
	query.exec("create table show "
			   "(id text primary key, "
			   "title text)");

	query.exec("create table season "
			   "(show_id text, "
			   "number integer)");

	return ret;
}

QSqlError DatabaseManager::lastError()
{
	// If opening database has failed user can ask
	// error description by QSqlError::text()
	return db.lastError();
}

DatabaseManager &DatabaseManager::instance()
{
	if (!_instance)
		_instance = new DatabaseManager();
	return *_instance;
}
