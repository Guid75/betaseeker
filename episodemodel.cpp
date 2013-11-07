//  Copyright 2013 Guillaume Denry (guillaume.denry@gmail.com)
//  This file is part of BetaSeeker.
//
//  BetaSeeker is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  BetaSeeker is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with BetaSeeker.  If not, see <http://www.gnu.org/licenses/>.

#include <QSqlRecord>
#include <QSqlTableModel>
#include <QColor>
#include <QDateTime>

#include "episodemodel.h"

SeasonListModel::SeasonListModel(QObject *parent) :
    QIdentityProxyModel(parent)
{
}

QVariant SeasonListModel::data(const QModelIndex &index, int role) const
{
    int secsFromEpoch;
    if (index.isValid() && index.column() == 0) {
        QSqlTableModel *m = qobject_cast<QSqlTableModel*>(sourceModel());
        QSqlRecord rec = m->record(index.row());
        switch (role) {
        case Qt::DisplayRole:
            return tr("Episode %1 (%2)").arg(rec.value("episode").toInt()).arg(rec.value("title").toString());
        case Qt::ForegroundRole:
            secsFromEpoch = rec.value("date").toInt();
            if (QDateTime::currentMSecsSinceEpoch() / 1000 >= secsFromEpoch)
                return QColor(Qt::black);
            return QColor(Qt::gray);
        default:
            return QIdentityProxyModel::data(index, role);
        }
    }

    return QVariant();
}

Qt::ItemFlags SeasonListModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
