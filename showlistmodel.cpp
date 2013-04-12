#include "showlistmodel.h"

ShowListModel::ShowListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int ShowListModel::rowCount(const QModelIndex & parent) const
{
	return _shows.count();
}

QVariant ShowListModel::data(const QModelIndex & index, int role) const
{
	const TvShow &show = _shows[index.row()];
	switch (role) {
	case Qt::DisplayRole:
		return show.title();
	}
	return QVariant();
}

void ShowListModel::addShow(const QString &title, const QString &url)
{
	if (indexOfShow(url) >= 0)
		return;

	beginInsertRows(QModelIndex(), _shows.count(), _shows.count());
	_shows << TvShow(title, url);
	endInsertRows();
}

int ShowListModel::indexOfShow(const QString &url)	const
{
	for (int i = 0; i < _shows.count(); i++) {
		const TvShow &show = _shows[i];
		if (show.url() == url)
			return i;
	}
	return -1;
}
