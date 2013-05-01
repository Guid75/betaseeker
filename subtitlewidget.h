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

#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include <QWidget>
#include <QSqlRecord>

class QLabel;

namespace Ui {
class SubtitleWidget;
}

class SubtitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubtitleWidget(QWidget *parent = 0);

    const QSqlRecord &record() const { return _record; }
    void setRecord(const QSqlRecord &record);

signals:
    
public slots:

private:
    Ui::SubtitleWidget *ui;
    QSqlRecord _record;
    
};

#endif // SUBTITLEWIDGET_H
