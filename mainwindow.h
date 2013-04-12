#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QUrl>

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
	void httpFinished();
	void httpReadyRead();

private:
	Ui::MainWindow *ui;
	int searchTimerId;
	QNetworkAccessManager network;
	QUrl url;
	QNetworkReply *reply;
	QString apiKey;

	//QString forgeRequest() const;
	void startRequest(QUrl url);
	void parseSearchResult(const QJsonObject &rootObj);
};

#endif // MAINWINDOW_H
