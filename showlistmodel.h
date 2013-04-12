#ifndef SHOWLISTMODEL_H
#define SHOWLISTMODEL_H

#include <QAbstractListModel>

#include "tvshow.h"

class ShowListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ShowListModel(QObject *parent = 0);
	
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	void addShow(const QString &title, const QString &url);

signals:
	
public slots:

private:
	QList<TvShow> _shows;

	int indexOfShow(const QString &url)	const;
};

#endif // SHOWLISTMODEL_H
