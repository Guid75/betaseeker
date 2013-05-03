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

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMessageBox>

#include "databasemanager.h"
#include "showmanager.h"
//#include "requestmanager.h"
#include "commandmanager.h"
#include "jsonparser.h"
#include "showdetailwidget.h"
#include "loadingwidget.h"
#include "settings.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow),    
    searchTimerId(0),
    ignoreCurrentPage(false)
{
	ui->setupUi(this);
	ui->splitterShows->setSizes(QList<int>() << 150);

	loadSettings();

    ui->tabWidgetMain->setCurrentWidget(ui->tabShows);
	ui->lineEditSearch->setFocus();

    connect(&CommandManager::instance(), &CommandManager::commandFinished,
            this, &MainWindow::commandFinished);

	connect(&ShowManager::instance(), &ShowManager::refreshDone,
			this, &MainWindow::refreshDone);

	QTimer::singleShot(0, Qt::CoarseTimer, this, SLOT(afterShow()));
}

MainWindow::~MainWindow()
{
	saveSettings();
	// TODO: save database
	QSqlDatabase::database().close();

	delete ui;
}

void MainWindow::afterShow()
{
	if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
		QMessageBox::warning(this, tr("Warning"), tr("No sqlite driver was found, no user data will be read or write"));
		return;
	}

	// read from an existing database
	switch (DatabaseManager::instance().openDB()) {
	case 0:
		QMessageBox::critical(this, tr("Warning"), tr("Cannot open neither the persistent cache database nor the memory database, you might have some serious resources problems, program cannot continue, bye!"));
		close();
		return;
	case 1:
		QMessageBox::warning(this, tr("Warning"), tr("Cannot open the persistent cache database, fallback to the memory data but all your cache data will be lost at the program closure"));
		break;
	default:
		break;
	}

	showListModel = new QSqlTableModel(this, QSqlDatabase::database());
	showListModel->setTable("show");
	showListModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    showListModel->setSort(showListModel->fieldIndex("title"), Qt::AscendingOrder);
	showListModel->select();
	showListModel->setHeaderData(0, Qt::Horizontal, tr("Title"));
	ui->listViewShows->setModel(showListModel);
	ui->listViewShows->setModelColumn(showListModel->fieldIndex("title"));
	ui->listViewShows->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect(ui->listViewShows->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &MainWindow::currentShowChanged);
}

void MainWindow::loadSettings()
{
}

void MainWindow::saveSettings()
{
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	QString value = ui->lineEditSearch->text();
	if (value.isEmpty())
		return;
    searchTicketId = CommandManager::instance().showsSearch(value);
	killTimer(event->timerId());
	searchTimerId = 0;

	if (searchTicketId == -1) {
		// TODO: manage overflow tickets
	}
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

void MainWindow::on_listWidgetSearch_itemDoubleClicked(QListWidgetItem *item)
{
	if (!item)
		return;

	QString id = item->data(Qt::UserRole).toString();
	QModelIndex index = getIndexByShowId(id);
	if (index.isValid()) {
		// already exists, just focus it
		ui->tabWidgetMain->setCurrentWidget(ui->tabShows);
		ui->listViewShows->setCurrentIndex(getIndexByShowId(id));
		return;
	}

	QSqlRecord record = QSqlDatabase::database().record("show");
    record.setValue("show_id", id);
	record.setValue("title", item->text());
	showListModel->insertRecord(-1, record);

	index = getIndexByShowId(id);
	if (index.isValid()) {
		ui->tabWidgetMain->setCurrentWidget(ui->tabShows);
		ui->listViewShows->setCurrentIndex(getIndexByShowId(id));
    }
}

void MainWindow::on_tabWidgetSeasons_currentChanged(int index)
{
    if (index < 0 || ignoreCurrentPage)
        return;

    // TODO refresh subtitles for a season
/*	ShowDetailWidget *widget = qobject_cast<ShowDetailWidget*>(ui->tabWidgetSeasons->widget(index));
    widget->loadSubtitles();*/
}

void MainWindow::on_pushButtonUnfollow_clicked()
{
    QModelIndex index = ui->listViewShows->currentIndex();
    if (!index.isValid())
        return;

    QSqlRecord record = showListModel->record(index.row());
    QString showId = record.value("show_id").toString();

    // remove all records from all tables which a certain show_id
    QSqlQuery query;
 /*   query.prepare("DELETE FROM show WHERE show_id=:show_id");
    query.bindValue(":show_id", showId);
    query.exec();*/

    query.prepare("DELETE FROM season WHERE show_id=:show_id");
    query.bindValue(":show_id", showId);
    query.exec();

    query.prepare("DELETE FROM episode WHERE show_id=:show_id");
    query.bindValue(":show_id", showId);
    query.exec();

    query.prepare("DELETE FROM subtitle WHERE show_id=:show_id");
    query.bindValue(":show_id", showId);
    query.exec();

    // finally, remove from the model itself
    if (showListModel->removeRows(index.row(), 1)) {
        showListModel->select();
        clearShowDetails();
    }

    // TODO manage error of removeRows
}

void MainWindow::commandFinished(int ticketId, const QByteArray &response)
{
	if (ticketId == searchTicketId)
		parseSearchResult(response);
}

void MainWindow::currentShowChanged(const QItemSelection &selected, const QItemSelection &)
{
	if (selected.count() == 0)
		return;

	QSqlRecord record = showListModel->record(selected.indexes()[0].row());

    // TODO we must check is a request is running yet for the current show and show the loading box in this case

    LoadingWidget::hideLoadingMask(ui->tabWidgetSeasons);
    switch (ShowManager::instance().refreshOnExpired(record.value("show_id").toString(), ShowManager::Item_Episodes)) {
    case 0:
        clearShowDetails();
        refreshShowDetails();
        break;
    case -1:
        // TODO manage error
        break;
    default:
        clearShowDetails();
        ui->tabWidgetSeasons->setDisabled(true);
        LoadingWidget::showLoadingMask(ui->tabWidgetSeasons);
        break;
    }
}

void MainWindow::refreshDone(const QString &url, ShowManager::Item item)
{
    if (url != getCurrentShowUrl())
        return;

    if (item == ShowManager::Item_Episodes)
        LoadingWidget::hideLoadingMask(ui->tabWidgetSeasons);
        ui->tabWidgetSeasons->setDisabled(false);
        refreshShowDetails();
}

QString MainWindow::getCurrentShowUrl() const
{
    QModelIndex index = ui->listViewShows->currentIndex();
    if (!index.isValid())
		return QString();

    QSqlRecord record = showListModel->record(index.row());
    return record.value("show_id").toString();
}

// A bit ugly, I don't like the idea to go throught ALL rows myself, maybe a Qt method exists
QModelIndex MainWindow::getIndexByShowId(const QString &id) const
{
	for (int row = 0; row < showListModel->rowCount(); row++) {
		QSqlRecord record = showListModel->record(row);
        if (record.value("show_id") == id)
			return showListModel->index(row, 0);
	}
	return QModelIndex();
}

void MainWindow::clearShowDetails()
{
    ignoreCurrentPage = true;
    // clear all tabs
    while (ui->tabWidgetSeasons->count()) {
        QWidget *widget = ui->tabWidgetSeasons->widget(0);
        delete widget;
    }
    ui->tabWidgetSeasons->clear();
    ignoreCurrentPage = false;
}

void MainWindow::refreshShowDetails()
{
    QModelIndex index = ui->listViewShows->currentIndex();
    if (!index.isValid())
        return;

    QSqlRecord record = showListModel->record(index.row());

    // create new tabs
    QSqlQuery query(QString("SELECT number FROM season WHERE show_id='%1'").arg(record.value("show_id").toString()));
    QList<int> numbers;
    while (query.next())
        numbers << query.value("number").toInt();
    qSort(numbers.begin(), numbers.end(), qGreater<int>());

    foreach (int number, numbers) {
        ShowDetailWidget *widget = new ShowDetailWidget;
		widget->init(record.value("show_id").toString(), number);
        ui->tabWidgetSeasons->addTab(widget, tr("Season %n", "", number));
    }
}

void MainWindow::parseSearchResult(const QByteArray &response)
{
	JsonParser parser(response);

	if (!parser.isValid()) {
		// TODO manage error
		return;
	}

	ui->listWidgetSearch->clear();

	QJsonObject shows = parser.root().value("shows").toObject();
	if (shows.isEmpty()) {
		// TODO manage error
		return;
	}

	foreach (const QString &key, shows.keys()) {
		QJsonObject show = shows.value(key).toObject();
		if (show.isEmpty()) {
			// TODO manage error
			continue;
		}

		QString title = show.value("title").toString();
		if (title.isNull()) {
			// TODO manage error
			continue;
		}
		QString url = show.value("url").toString();
		if (url.isNull()) {
			// TODO manage error
			continue;
		}
		QListWidgetItem *item = new QListWidgetItem(title);
		item->setData(Qt::UserRole, url);
		ui->listWidgetSearch->addItem(item);
	}
}
