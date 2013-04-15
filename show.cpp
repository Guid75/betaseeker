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
