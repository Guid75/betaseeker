#ifndef TVSHOW_H
#define TVSHOW_H

#include <QString>
#include <QJsonObject>

#include "season.h"

class Show
{
public:
	enum ShowItem {
		Item_Episodes,
		Item_Subtitles
	};

	Show(const QString &title, const QString &url);

	const QString title() const { return _title; }
	const QString url() const { return _url; }

	const Season &seasonAt(int index) const;
    int seasonCount() const;

    void parseEpisodes(const QJsonObject &root);

private:
	QString _title;
	QString _url;
	QList<Season*> _seasons;

    Season *getSeasonByNumber(int number) const;
};

#endif // TVSHOW_H
