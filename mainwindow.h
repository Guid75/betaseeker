#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSqlTableModel>

#include "showmanager.h"

namespace Ui {
class MainWindow;
class ShowDetail;
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

    void requestFinished(int ticketId, const QByteArray &response);
	void currentShowChanged(const QItemSelection &selected, const QItemSelection &);
    void refreshDone(const QString &url, ShowManager::Item item);
    void afterShow();

private:
	Ui::MainWindow *ui;
    Ui::ShowDetail *showDetailUi;
    int searchTicketId;
	int searchTimerId;
    //ShowListModel *showListModel;
    QSqlTableModel *showListModel;

	void loadSettings();
	void saveSettings();
    void parseSearchResult(const QByteArray &response);
    void refreshShowDetails();
    QString getCurrentShowUrl() const;
    QModelIndex getIndexByShowId(const QString &id) const;
};

#endif // MAINWINDOW_H
