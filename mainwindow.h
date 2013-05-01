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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSqlTableModel>

#include "showmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
protected:
	virtual void timerEvent(QTimerEvent *event);

private slots:
	void on_lineEditSearch_textChanged(const QString &text);
	void on_listWidgetSearch_itemDoubleClicked(QListWidgetItem *item);
    void on_tabWidgetSeasons_currentChanged(int index);
    void on_pushButtonUnfollow_clicked();

    void requestFinished(int ticketId, const QByteArray &response);
	void currentShowChanged(const QItemSelection &selected, const QItemSelection &);
    void refreshDone(const QString &url, ShowManager::Item item);
    void afterShow();

private:
	Ui::MainWindow *ui;
    int searchTicketId;
	int searchTimerId;
    QSqlTableModel *showListModel;
    bool ignoreCurrentPage;

	void loadSettings();
	void saveSettings();
    void parseSearchResult(const QByteArray &response);
    void clearShowDetails();
    void refreshShowDetails();
    QString getCurrentShowUrl() const;
    QModelIndex getIndexByShowId(const QString &id) const;
};

#endif // MAINWINDOW_H
