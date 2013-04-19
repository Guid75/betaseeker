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
    if (!DatabaseManager::instance().openDB()) {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open the database"));
        return;
    }

    //showListModel = new ShowListModel(this);
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

    ShowManager::instance().populateFromDB();
}

void MainWindow::loadSettings()
{
	QSettings settings("Guid75", QCoreApplication::instance()->applicationName());
	settings.beginGroup("General");
	settings.endGroup();

//	settings.beginGroup("Shows");
//	QStringList shows = settings.childGroups();
//	foreach (const QString &showUrl, settings.childGroups()) {
//		settings.beginGroup(showUrl);
//		ShowManager::instance().addShow(settings.value("title").toString(), showUrl);
//		settings.endGroup();
//	}

//	settings.endGroup();
}

void MainWindow::saveSettings()
{
	return;
	QSettings settings("Guid75", QCoreApplication::instance()->applicationName());
	settings.beginGroup("General");
	settings.endGroup();

    // settings.beginGroup("Shows");
	//	foreach (const TvShow &show, _shows) {
	//		settings.beginGroup(show.url());
	//		settings.setValue("title", show.title());
	//		settings.endGroup();
	//	}

    // settings.endGroup();
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

    ShowManager::instance().load(record.value("id").toString(), Show::Item_Episodes);
//    ShowManager::instance().refresh(show.url(), Show::Item_Episodes);
}

void MainWindow::refreshDone(const QString &url, Show::ShowItem item)
{
    if (url != getCurrentShowUrl())
        return;

    if (item == Show::Item_Episodes)
        refreshComboBoxes();
}

QString MainWindow::getCurrentShowUrl() const
{
    QModelIndexList selected = ui->listViewShows->selectionModel()->selectedRows();
    if (selected.count() == 0)
        return QString();

    const Show &show = ShowManager::instance().showAt(selected[0].row());
    return show.url();
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

void MainWindow::refreshComboBoxes()
{
    QModelIndexList selected = ui->listViewShows->selectionModel()->selectedRows();
    if (selected.count() == 0)
        return;

    const Show &show = ShowManager::instance().showAt(selected[0].row());

    ui->comboBoxSeason->clear();
    ui->comboBoxSeason->addItem(tr("<Unspecified>"));
    for (int i = 0; i < show.seasonCount(); i++)
        ui->comboBoxSeason->addItem(tr("Season %1").arg(show.seasonAt(i).number()));
    ui->comboBoxSeason->update();
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
