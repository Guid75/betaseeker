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
