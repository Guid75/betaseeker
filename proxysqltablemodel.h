#ifndef PROXYSQLTABLEMODEL_H
#define PROXYSQLTABLEMODEL_H

#include <QSqlTableModel>

class ProxySqlTableModel : public QSqlTableModel
{
	Q_OBJECT
public:
	explicit ProxySqlTableModel(QObject *pObject = 0);
	QVariant data (const QModelIndex & Index, int iRole = Qt::DisplayRole) const;
	void setProxyModel(const QString& sTable);
};

#endif // PROXYSQLTABLEMODEL_H
