#include <QSqlRecord>
#include <QSqlQuery>

#include "subtitlesmodel.h"

struct Episode
{
	QSqlRecord episodeRecord;
	QList<QSqlRecord> subtitles;
};

SubtitlesModel::SubtitlesModel(QObject *parent) :
	QAbstractItemModel(parent)
{

}

SubtitlesModel::~SubtitlesModel()
{
	qDeleteAll(episodes);
}

void SubtitlesModel::init(const QString &showId, int season)
{
	beginResetModel();
	qDeleteAll(episodes);

	QSqlQuery query;

	query.prepare(QString("SELECT episode FROM episode WHERE show_id='%1' AND season=%2").arg(showId).arg(season));
	query.exec();

	QList<int> eps;
	while (query.next()) {
		eps << query.value("episode").toInt();
	}
	qSort(eps.begin(), eps.end(), qGreater<int>());

	foreach (int ep, eps) {
		query.prepare(QString("SELECT * FROM episode WHERE show_id='%1' AND season=%2 AND episode=%3").arg(showId).arg(season).arg(ep));
		query.exec();

		if (query.next()) {
			Episode *episode = new Episode;
			episode->episodeRecord = query.record();
			episodes << episode;
		}
	}
	endResetModel();
}

QModelIndex SubtitlesModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, (void*) 0);
}

QModelIndex SubtitlesModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int SubtitlesModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return episodes.count();
	else
		return 0;
}

int SubtitlesModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant SubtitlesModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	Episode *episode = episodes[index.row()];

	switch (role) {
	case Qt::DisplayRole:
		if (index.column() == 0)
			return tr("Episode %1 (%2)").arg(episode->episodeRecord.value("episode").toInt()).arg(episode->episodeRecord.value("title").toString());
		break;
	default:
		break;
	}

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
