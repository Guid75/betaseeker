#ifndef TVSHOW_H
#define TVSHOW_H

#include <QString>

class Show
{
public:
    enum ShowItemType {
        Item_Resume,
        Item_Subtitles
    };

    Show(const QString &title, const QString &url);

	const QString title() const { return _title; }
	const QString url() const { return _url; }

private:
	QString _title;
	QString _url;
};

#endif // TVSHOW_H
