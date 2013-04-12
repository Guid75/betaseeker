#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#include "showlistmodel.h"

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
	void httpFinished();
	void httpReadyRead();
	void httpError(QNetworkReply::NetworkError);
	void displayApiKeyDialog();

private:
	Ui::MainWindow *ui;
	int searchTimerId;
	QNetworkAccessManager network;
	QUrl url;
	QNetworkReply *reply;
	ShowListModel *showListModel;

	void loadSettings();
	void saveSettings();
	//QString forgeRequest() const;
	void startRequest(QUrl url);
	void parseSearchResult(const QJsonObject &rootObj);
};

#endif // MAINWINDOW_H
