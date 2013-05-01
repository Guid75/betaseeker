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
