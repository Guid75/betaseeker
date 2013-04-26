#ifndef SUBTITLESMODEL_H
#define SUBTITLESMODEL_H

#include <QAbstractItemModel>

class Episode;

class SubtitlesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit SubtitlesModel(QObject *parent = 0);
    
    void init(const QString &showId, int season);

    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & index) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
signals:
    
public slots:

private:
    QList<Episode*> episodes;
};

#endif // SUBTITLESMODEL_H
