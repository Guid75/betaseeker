#ifndef SEASON_H
#define SEASON_H

#include <QList>
#include <QJsonObject>

#include "episode.h"

class Season
{
public:
	Season(int number);

	int number() const { return _number; }

	int episodeCount();
	const Episode &episodeAt(int index);

    void parseEpisodes(const QJsonObject &root);

private:
	int _number;
	QList<Episode*> _episodes;

    Episode *getEpisodeByEpisode(int episode) const;
};

#endif // SEASON_H
