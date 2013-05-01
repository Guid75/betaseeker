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
	QSqlQuery query;

    // shows
    query.exec("CREATE TABLE show (show_id text primary key, title text, episodes_last_check_date integer)");

    // seasons
    query.exec("CREATE TABLE season (show_id text, number integer, PRIMARY KEY (show_id, number))");

    // episodes
    query.exec("CREATE TABLE episode "
               "(show_id text, season integer, episode integer, title text, "
               "number text, global integer, date integer, "
               "comments integer, subtitles_last_check_data integer, PRIMARY KEY (show_id, season, episode))");

//    <subtitle>
//    <title>Dexter</title>
//    <season>1</season>
//    <episode>0</episode>
//    <language>VF</language>
//    <source>soustitres</source>
//    <file>Dexter.S01.DVDRip.SAiNTS.FR.zip</file>
//    <content>
//    <item>Dexter.S01E01.Dexter.RETAiL.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E02.Crocodile.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E03.Popping.Cherry.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E04.Lets.Give.The.Boy.A.Hand.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E05.Love.American.Style.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E06.Return.To.Sender.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E07.Circle.Of.Friends.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E08.Shrink.Wrap.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E09.Father.Knows.Best.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E10.Seeing.Red.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E11.Truth.Be.Told.DVDRip.XviD-SAiNTS.srt</item>
//    <item>Dexter.S01E12.Born.Free.DVDRip.XviD-SAiNTS.srt</item>
//    <item/>
//    </content>
//    <url>https://www.betaseries.com/srt/186083</url>
//    <quality>5</quality>
//    </subtitle>

    // subtitles
    // TODO add content which is a list of filenames. How could be store that? Another table?
    query.exec("CREATE TABLE subtitle "
               "(show_id text, season integer, episode integer, "
               "language text, source text, file text, "
               "url text, quality integer)");

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
