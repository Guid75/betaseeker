#include <QStringList>

#include "season.h"

Season::Season(int number) :
	_number(number)
{
}

int Season::episodeCount()
{
	return _episodes.count();
}

const Episode &Season::episodeAt(int index)
{
	return *_episodes[index];
}

void Season::parseEpisodes(const QJsonObject &root)
{
    QJsonObject episodesJson = root.value("episodes").toObject();
    foreach (const QString &key, episodesJson.keys()) {
        QJsonObject episodeJson = episodesJson.value(key).toObject();
        if (episodeJson.isEmpty())
            continue;

        // numbers are under the form of strings from betaseries :/
        bool ok;
        int episodeEp = episodeJson.value("episode").toString().toInt(&ok);
        if (!ok)
            continue;

        Episode *episode = getEpisodeByEpisode(episodeEp);
        if (episode == 0) {
            episode = new Episode(episodeEp);
            _episodes << episode;
        }
        episode->parseEpisode(episodeJson);
    }
}

Episode *Season::getEpisodeByEpisode(int epi) const
{
    foreach (Episode *episode, _episodes) {
        if (episode->episode() == epi)
            return episode;
    }
    return 0;
}
