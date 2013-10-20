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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSize>

namespace Settings {

QString directoryForSeason(const QString &showId, int season);
void setDirectoryForSeason(const QString &showId, int season, const QString &filePath);
QSize mainWindowSize();
void setMainWindowSize(const QSize &size);
bool mainWindowMaximized();
void setMainWindowMaximized(bool maximized);
}

#endif // SETTINGS_H
