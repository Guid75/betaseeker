#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "mainwindow.h"
#include "ui_mainwindow.h"

static const QString websiteUrl = "http://api.betaseries.com";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	searchTimerId(0),
	apiKey("9adb4ab628c6")
{
	ui->setupUi(this);
	ui->tabWidgetMain->setCurrentIndex(0);
	ui->lineEditSearch->setFocus();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	QString value = ui->lineEditSearch->text();
	qDebug("Launch search on %s", qPrintable(value));
	url = QString("%1/shows/search.json?title=%2&key=%3").arg(websiteUrl).arg(value).arg(apiKey);
	startRequest(url);
	killTimer(event->timerId());
	searchTimerId = 0;
}

void MainWindow::on_lineEditSearch_textChanged(const QString &text)
{
	if (searchTimerId > 0) {
		killTimer(searchTimerId);
		searchTimerId = 0;
	}

	if (text.length() > 2)
		searchTimerId = startTimer(1000);
}

void MainWindow::startRequest(QUrl url)
{
	reply = network.get(QNetworkRequest(url));
	connect(reply, SIGNAL(finished()),
			this, SLOT(httpFinished()));
	connect(reply, SIGNAL(readyRead()),
			this, SLOT(httpReadyRead()));
/*	connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
			this, SLOT(updateDataReadProgress(qint64,qint64)));*/
}

void MainWindow::httpFinished()
{
	reply->deleteLater();
}

void MainWindow::httpReadyRead()
{
	QByteArray response = reply->readAll();
	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(response, &error);
	if (jsonDoc.isNull()) {
		qDebug("error while parsing json");
		qDebug("%s: %d", qPrintable(error.errorString()), error.offset);
		return;
	}

	if (!jsonDoc.isObject()) {
		// TODO raise error, we except an object
		return;
	}

	QJsonObject obj = jsonDoc.object();
	QJsonValue val = obj.value("root");
	parseSearchResult(val.toObject());
}

void MainWindow::parseSearchResult(const QJsonObject &rootObj)
{
	ui->listWidgetSearch->clear();

	QJsonValue val = rootObj.value("shows");
	if (!val.isObject()) {
		// TODO manage error
		return;
	}

	QJsonObject shows = val.toObject();
	foreach (const QString &key, shows.keys()) {
		val = shows.value(key);
		if (!val.isObject()) {
			// TODO manage error
			continue;
		}
		QJsonObject show = val.toObject();
		val = show.value("title");
		if (!val.isString()) {
			// TODO manage error
			continue;
		}
		QString title = val.toString();
		val = show.value("url");
		if (!val.isString()) {
			// TODO manage error
			continue;
		}
		QString url = val.toString();
		QListWidgetItem *item = new QListWidgetItem(title);
		item->setData(Qt::UserRole, url);
		ui->listWidgetSearch->addItem(item);

		qDebug("%s:%s", qPrintable(title), qPrintable(url));
	}
}
