#include "seasonwidget.h"

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
}

void ShowDetailWidget::loadSubtitles()
{
//	subtitlesModel->init(_showId, _season);
}
