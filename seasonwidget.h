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

#ifndef SEASONWIDGET_H
#define SEASONWIDGET_H

#include <QWidget>
#include <QMap>

class EpisodeWidget;

class SeasonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SeasonWidget(QWidget *parent = 0);
    
    void init(const QString &showId, int season);

signals:
    
public slots:
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QString _showId;
    int _season;
    QMap<int,EpisodeWidget*> tickets;

    void clear();
    void parseSubtitles(EpisodeWidget *episodeWidget, const QByteArray &response);

private slots:
    void requestFinished(int ticketId, const QByteArray &response);
};

#endif // SUBTITLESWIDGET_H
