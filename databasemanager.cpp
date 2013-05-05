//  Copyright 2013 Guillaume Denry (guillaume.denry@gmail.com)
//  This file is part of BetaSeeker.
//
//  BetaSeeker is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  BetaSeeker is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with BetaSeeker.  If not, see <http://www.gnu.org/licenses/>.

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
    // TODO: use ids for all tables
	QSqlQuery query;

    query.exec("PRAGMA foreign_keys = ON;");

    // shows
    query.exec("CREATE TABLE show (show_id text primary key, title text, episodes_last_check_date integer)");

    // seasons
    query.exec("CREATE TABLE season (show_id text, number integer, PRIMARY KEY (show_id, number))");

    // episodes
    query.exec("CREATE TABLE episode "
               "(show_id text, season integer, episode integer, title text, "
               "number text, global integer, date integer, "
               "comments integer, subtitles_last_check_date integer, PRIMARY KEY (show_id, season, episode))");

    // subtitles
    query.exec("CREATE TABLE subtitle "
               "(id integer primary key, show_id text, season integer, episode integer, "
               "language text, source text, file text, "
               "url text, quality integer, "
               "UNIQUE (show_id, season, episode, url) ON CONFLICT REPLACE)");

    // subtitles content
    query.exec("CREATE TABLE subtitle_content "
               "(subtitle_id integer, file text, "
               "FOREIGN KEY (subtitle_id) REFERENCES subtitle(id) ON DELETE CASCADE, "
               "UNIQUE (subtitle_id, file) ON CONFLICT REPLACE)");

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
