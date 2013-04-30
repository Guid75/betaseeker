#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace Settings {

QString directoryForSeason(const QString &showId, int season);
void setDirectoryForSeason(const QString &showId, int season, const QString &filePath);

}

#endif // SETTINGS_H
