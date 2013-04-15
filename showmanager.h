#ifndef SHOWMANAGER_H
#define SHOWMANAGER_H

#include <QObject>
#include <QHash>

#include "show.h"

class ShowManager : public QObject
{
	Q_OBJECT
public:
	static ShowManager &instance();

	int showsCount() const { return _shows.count(); }
	const Show &showAt(int index) const;
	Show &showAt(int index);
	void addShow(const QString &title, const QString &url);
	int indexOfShow(const QString &url) const;

	void refresh(const QString &url, Show::ShowItem item);

signals:
	void showAdded(const Show &show);

public slots:

private:
	typedef void (*parse_func)(const QString &, const QByteArray &);

	static ShowManager *_instance;
	QList<Show*> _shows;
	QHash<int,QPair<QString,parse_func> > parsing;

	explicit ShowManager();
	Show *showAt(const QString &url);

	// parsing methods
	static void parseEpisodes(const QString &url, const QByteArray &response);

private slots:
	void requestFinished(int ticketId, const QByteArray &response);
};

#endif // SHOWMANAGER_H
