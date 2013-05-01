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

#ifndef SHOWDETAILWIDGET_H
#define SHOWDETAILWIDGET_H

#include <QWidget>
#include <QItemSelection>

class SubtitlesModel;
class SeasonWidget;
class QSqlTableModel;
class EpisodeModel;

namespace Ui {
class ShowDetailWidget;
}

class ShowDetailWidget : public QWidget
{
    Q_OBJECT
public:
	explicit ShowDetailWidget(QWidget *parent = 0);

	void init(const QString &showId, int season);

signals:

public slots:

private:
	QString _showId;
	int _season;
    Ui::ShowDetailWidget *ui;
    int subtitleTicket;
    SubtitlesModel *subtitlesModel;
    SeasonWidget *seasonWidget;
    QSqlTableModel *episodeModel;
    EpisodeModel *episodeProxyModel;
    QSqlTableModel *subtitleModel;
    QMap<int, int> tickets;

    void parseSubtitles(int episode, const QByteArray &response);

private slots:
    void on_pushButtonDefineIt_clicked();
    void on_pushButtonForgetIt_clicked();

    void currentEpisodeChanged(const QItemSelection &selected, const QItemSelection &);
    void requestFinished(int ticketId, const QByteArray &response);

};

#endif // SHOWDETAILWIDGET_H
