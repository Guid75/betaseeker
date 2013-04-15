#ifndef SEASON_H
#define SEASON_H

#include <QList>

#include "episode.h"

class Season
{
public:
	Season(int number);

	int number() const { return _number; }

	int episodeCount();
	const Episode &episodeAt(int index);

private:
	int _number;
	QList<Episode*> _episodes;
};

#endif // SEASON_H
