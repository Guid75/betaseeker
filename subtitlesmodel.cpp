#include "subtitlesmodel.h"

struct Episode
{

};

SubtitlesModel::SubtitlesModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}

QModelIndex SubtitlesModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column, 5);
}

QModelIndex SubtitlesModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int SubtitlesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 5;
    else
        return 0;
}

int SubtitlesModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant SubtitlesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return "Cell value with a very long title";
    return QVariant();
}

bool SubtitlesModel::hasChildren(const QModelIndex &parent) const
{
    return true;
}

QVariant SubtitlesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        return "Header";
    return QVariant();
}
