#include "showlistmodel.h"
#include "showmanager.h"

ShowListModel::ShowListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    connect(&ShowManager::instance(), SIGNAL(showAdded(TvShow)),
            this, SLOT(showAdded(TvShow)));
}

int ShowListModel::rowCount(const QModelIndex &) const
{
    return ShowManager::instance().showsCount();
}

QVariant ShowListModel::data(const QModelIndex & index, int role) const
{
    const TvShow &show = ShowManager::instance().showAt(index.row());
	switch (role) {
	case Qt::DisplayRole:
		return show.title();
	}
	return QVariant();
}

void ShowListModel::showAdded(const TvShow &show)
{
    beginInsertRows(QModelIndex(), ShowManager::instance().showsCount() - 1, ShowManager::instance().showsCount() - 1);
    endInsertRows();
}
