#include <QFileDialog>

#include "seasonwidget.h"
#include "settings.h"

#include "ui_showdetailwidget.h"
#include "showdetailwidget.h"

ShowDetailWidget::ShowDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDetailWidget)
{
    ui->setupUi(this);

    seasonWidget = new SeasonWidget;
    ui->scrollArea->setWidget(seasonWidget);
}

void ShowDetailWidget::init(const QString &showId, int season)
{
    _showId = showId;
	_season = season;
    seasonWidget->init(_showId, _season);

    ui->widgetSeasonDir->setVisible(Settings::directoryForSeason(_showId, _season).isEmpty());
}

void ShowDetailWidget::on_pushButtonDefineIt_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, tr("Pick your show season directory"));
    if (filePath.isEmpty())
        return;
    Settings::setDirectoryForSeason(_showId, _season, filePath);
    ui->widgetSeasonDir->hide();
}

void ShowDetailWidget::on_pushButtonForgetIt_clicked()
{
    ui->widgetSeasonDir->hide();
}
