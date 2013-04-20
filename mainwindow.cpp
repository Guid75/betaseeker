#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMessageBox>

#include "databasemanager.h"
#include "showmanager.h"
#include "requestmanager.h"
#include "jsonparser.h"
#include "showdetailwidget.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow),
	searchTimerId(0)
{
	ui->setupUi(this);
	ui->splitterShows->setSizes(QList<int>() << 150);

	loadSettings();

	ui->tabWidgetMain->setCurrentWidget(ui->tabSearch);
	ui->lineEditSearch->setFocus();

	connect(&RequestManager::instance(), &RequestManager::requestFinished,
			this, &MainWindow::requestFinished);

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
	QSettings settings("Guid75", QCoreApplication::instance()->applicationName());
	settings.beginGroup("General");
	settings.endGroup();
}

void MainWindow::saveSettings()
{
	return;
	QSettings settings("Guid75", QCoreApplication::instance()->applicationName());
	settings.beginGroup("General");
	settings.endGroup();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	QString value = ui->lineEditSearch->text();
	if (value.isEmpty())
		return;
	searchTicketId = RequestManager::instance().showsSearch(value);
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
	record.setValue("id", id);
	record.setValue("title", item->text());
	showListModel->insertRecord(-1, record);

	index = getIndexByShowId(id);
	if (index.isValid()) {
		ui->tabWidgetMain->setCurrentWidget(ui->tabShows);
		ui->listViewShows->setCurrentIndex(getIndexByShowId(id));
	}
}

void MainWindow::requestFinished(int ticketId, const QByteArray &response)
{
	if (ticketId == searchTicketId)
		parseSearchResult(response);
}

void MainWindow::currentShowChanged(const QItemSelection &selected, const QItemSelection &)
{
	if (selected.count() == 0)
		return;

	QSqlRecord record = showListModel->record(selected.indexes()[0].row());
	//	const Show &show = ShowManager::instance().showAt(showIndex);

    switch (ShowManager::instance().refreshOnExpired(record.value("id").toString(), ShowManager::Item_Episodes)) {
    case 0:
        refreshShowDetails();
        break;
    case -1:
        // TODO manage error
        break;
    default:
        break;
    }
}

void MainWindow::refreshDone(const QString &url, ShowManager::Item item)
{
    if (url != getCurrentShowUrl())
        return;

    if (item == ShowManager::Item_Episodes)
        refreshShowDetails();
}

QString MainWindow::getCurrentShowUrl() const
{
    QModelIndex index = ui->listViewShows->currentIndex();
    if (!index.isValid())
		return QString();

    QSqlRecord record = showListModel->record(index.row());
    return record.value("id").toString();
}

// A bit ugly, I don't like the idea to go throught ALL rows myself, maybe a Qt method exists
QModelIndex MainWindow::getIndexByShowId(const QString &id) const
{
	for (int row = 0; row < showListModel->rowCount(); row++) {
		QSqlRecord record = showListModel->record(row);
		if (record.value("id") == id)
			return showListModel->index(row, 0);
	}
	return QModelIndex();
}

void MainWindow::refreshShowDetails()
{
    QModelIndex index = ui->listViewShows->currentIndex();
    if (!index.isValid())
        return;

    QSqlRecord record = showListModel->record(index.row());

    // clear all tabs
    while (ui->tabWidgetSeasons->count()) {
        QWidget *widget = ui->tabWidgetSeasons->widget(0);
        delete widget;
    }
    ui->tabWidgetSeasons->clear();

    // create new tabs
    QSqlQuery query(QString("SELECT number FROM season WHERE show_id='%1'").arg(record.value("id").toString()));
    QList<int> numbers;
    while (query.next())
        numbers << query.value("number").toInt();
    qSort(numbers.begin(), numbers.end(), qGreater<int>());

    foreach (int number, numbers) {
        ShowDetailWidget *widget = new ShowDetailWidget;
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
