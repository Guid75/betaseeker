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

#include <QCoreApplication>
#include <QSettings>

#include "settings.h"

namespace Settings {

#define DEFINE_SETTINGS QSettings settings("Guid75", QCoreApplication::instance()->applicationName())


QString directoryForSeason(const QString &showId, int season)
{
    DEFINE_SETTINGS;

    return settings.value(QString("shows/%1/season%2/filepath").arg(showId).arg(season), "").toString();
}

void setDirectoryForSeason(const QString &showId, int season, const QString &filePath)
{
    DEFINE_SETTINGS;

    settings.setValue(QString("shows/%1/season%2/filepath").arg(showId).arg(season), filePath);
}

QSize mainWindowSize()
{
    DEFINE_SETTINGS;

    QSize defaultSize(-1, -1);
    return settings.value("mainWindow/size", defaultSize).toSize();
}

void setMainWindowSize(const QSize &size)
{
    DEFINE_SETTINGS;

    settings.setValue("mainWindow/size", size);
}

bool mainWindowMaximized()
{
    DEFINE_SETTINGS;

    return settings.value("mainWindow/maximized", false).toBool();
}

void setMainWindowMaximized(bool maximized)
{
    DEFINE_SETTINGS;

    settings.setValue("mainWindow/maximized", maximized);
}

}
