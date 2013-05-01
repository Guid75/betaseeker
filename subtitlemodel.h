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

#ifndef SUBTITLEMODEL_H
#define SUBTITLEMODEL_H

#include <QIdentityProxyModel>

class SubtitleModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit SubtitleModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &) const;

signals:
    
public slots:
    
};

#endif // SUBTITLEMODEL_H
