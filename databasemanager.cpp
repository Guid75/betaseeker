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

    QDir dir = QDir::home();
    if (dir.mkpath(".betaseeker") && dir.cd(".betaseeker")) {
        db.setDatabaseName(QDir::toNativeSeparators(dir.filePath("shows.sqlite")));
    }

	int ret = 2;
    // let try to open the persistent database
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

    // shows
    query.exec("CREATE TABLE show (id text primary key, title text, episodes_last_check_date integer)");

    // seasons
    query.exec("CREATE TABLE season (show_id text, number integer, PRIMARY KEY (show_id, number))");

    // episodes
    query.exec("CREATE TABLE episode "
               "(show_id text, season integer, episode integer, title text, "
               "number text, global integer, date integer, "
               "comments integer, PRIMARY KEY (show_id, season, episode))");

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
