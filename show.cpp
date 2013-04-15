#include <QStringList>

#include "show.h"

Show::Show(const QString &title, const QString &url) :
	_title(title),
	_url(url)
{
}

const Season &Show::seasonAt(int index) const
{
    return *_seasons[index];
}

int Show::seasonCount() const
{
    return _seasons.count();
}

void Show::parseEpisodes(const QJsonObject &root)
{
    QJsonObject seasonsJson = root.value("seasons").toObject();
    foreach (const QString &key, seasonsJson.keys()) {
        QJsonObject seasonJson = seasonsJson.value(key).toObject();
        if (seasonJson.isEmpty())
            continue;

        // numbers are under the form of strings from betaseries :/
        bool ok;
        int number = seasonJson.value("number").toString().toInt(&ok);
        if (!ok)
            continue;

        Season *season = getSeasonByNumber(number);
        if (!season) {
            season = new Season(number);
            _seasons << season;
        }
        season->parseEpisodes(seasonJson);
    }
}

Season *Show::getSeasonByNumber(int number) const
{
    foreach (Season *season, _seasons) {
        if (season->number() == number)
            return season;
    }
    return 0;
}
