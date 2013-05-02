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

#include "subtitlemodel.h"

SubtitleModel::SubtitleModel(QObject *parent) :
    QIdentityProxyModel(parent)
{
}

QVariant SubtitleModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
      return QIdentityProxyModel::data(index, role);

    if (index.isValid() && index.column() == 0) {
        QSqlTableModel *m = qobject_cast<QSqlTableModel*>(sourceModel());
        QSqlRecord rec = m->record(index.row());
        return QString("[%1] %2").arg(rec.value("quality").toInt()).arg(rec.value("file").toString());
//        return QString("<a href=\"www.lemonde.fr\">[%1] %2</a>").arg(rec.value("quality").toInt()).arg(rec.value("file").toString());
    }

    return QVariant();
}

Qt::ItemFlags SubtitleModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
