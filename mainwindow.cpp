#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTimer>

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
	ui->splitterMain->setSizes(QList<int>() << 75);

	showListModel = new ShowListModel(this);
	ui->listViewShows->setModel(showListModel);

	loadSettings();

	ui->tabWidgetMain->setCurrentWidget(ui->tabSearch);
	ui->lineEditSearch->setFocus();

	connect(ui->listViewShows->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &MainWindow::currentShowChanged);

	connect(&RequestManager::instance(), &RequestManager::requestFinished,
			this, &MainWindow::requestFinished);

    connect(&ShowManager::instance(), &ShowManager::refreshDone,
            this, &MainWindow::refreshDone);
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete ui;
}

void MainWindow::loadSettings()
{
	QSettings settings("Guid75", QCoreApplication::instance()->applicationName());
	settings.beginGroup("General");
	settings.endGroup();

	settings.beginGroup("Shows");
	QStringList shows = settings.childGroups();
	foreach (const QString &showUrl, settings.childGroups()) {
		settings.beginGroup(showUrl);
		ShowManager::instance().addShow(settings.value("title").toString(), showUrl);
		settings.endGroup();
	}

	settings.endGroup();
}

void MainWindow::saveSettings()
{
	return;
	QSettings settings("Guid75", QCoreApplication::instance()->applicationName());
	settings.beginGroup("General");
	settings.endGroup();

	settings.beginGroup("Shows");
	//	foreach (const TvShow &show, _shows) {
	//		settings.beginGroup(show.url());
	//		settings.setValue("title", show.title());
	//		settings.endGroup();
	//	}

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

	ShowManager::instance().addShow(item->text(), item->data(Qt::UserRole).toString());
	ui->tabWidgetMain->setCurrentWidget(ui->tabShows);
	ui->listViewShows->selectionModel()->select(showListModel->index(ShowManager::instance().showsCount() - 1, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Clear);
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

	int showIndex = selected.indexes()[0].row();
	const Show &show = ShowManager::instance().showAt(showIndex);

    ShowManager::instance().refresh(show.url(), Show::Item_Episodes);
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
