#ifndef EPISODE_H
#define EPISODE_H

#include <QString>

class Episode
{
public:
	Episode(int number);

	int number() const { return _number; }
	const QString &title() const { return _title; }

private:
	int _number;
	QString _title;
};

#endif // EPISODE_H
