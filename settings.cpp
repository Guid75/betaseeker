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

}
