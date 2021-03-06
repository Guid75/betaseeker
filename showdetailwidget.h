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
class SeasonListModel;
class QStandardItemModel;
class EpisodeFinder;
class QStandardItem;

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

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
	QString _showId;
	int _season;
    int _episode;
    Ui::ShowDetailWidget *ui;
    int subtitleTicket;
    SubtitlesModel *subtitlesModel;
    SeasonWidget *seasonWidget;
    QSqlTableModel *episodeModel;
    SeasonListModel *episodeProxyModel;
    QStandardItemModel *subtitleModel;
    EpisodeFinder *episodeFinder;
    QMap<int, int> tickets;
	int downloadTicket;
    QStandardItem *localRoot;

    void parseSubtitles(int episode, const QByteArray &response);
    void refreshSubtitleTree(int episode);
    void refreshEpisodesComboBox();
    void renameAccordingToCurrentVideoFile(const QString &filePath);
    void findExistingAndRenameIt(const QString &filePath);
    static QString replaceExtension(const QString &fileName, const QString &newExtension);
    QString getComboBoxFile() const;

    QByteArray getFileHash(const QString &fileName) const;

    void refreshLocalSubtitles();

private slots:
    void on_pushButtonDefineIt_clicked();
    void on_pushButtonForgetIt_clicked();
    void on_toolButtonRefreshSubtitles_clicked();
    void on_toolButtonPlay_clicked();

    void currentEpisodeChanged(const QItemSelection &selected, const QItemSelection &);
    void commandFinished(int ticketId, const QByteArray &response);
    void downloadFinished(int ticketId, const QString &filePath, const QVariant &userData);
    void linkClicked(const QModelIndex &index);
};

#endif // SHOWDETAILWIDGET_H
