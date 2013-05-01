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

#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class LoadingWidget;
}

// ugly! this class should be private, but I can't for now because Q_OBJECT need to be collected by moc
class LoadingEventManager : public QObject
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class LoadingWidget : public QWidget
{
    Q_OBJECT

    friend class LoadingEventManager;
public:
    explicit LoadingWidget(QWidget *parent = 0);

    static void showLoadingMask(QWidget *widget);
    static void hideLoadingMask(QWidget *widget);

signals:
    
public slots:

protected:
    void timerEvent(QTimerEvent *);

private:
    Ui::LoadingWidget *ui;
    int labelState;

    void computePosition();
    
};

#endif // LOADINGWIDGET_H
