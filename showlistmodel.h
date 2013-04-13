#ifndef SHOWLISTMODEL_H
#define SHOWLISTMODEL_H

#include <QAbstractListModel>

#include "show.h"

class ShowListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ShowListModel(QObject *parent = 0);
	
    int rowCount(const QModelIndex & = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:
	
public slots:

private slots:
    void showAdded(const Show &show);
};

#endif // SHOWLISTMODEL_H
