#include "subtitlesmodel.h"

#include "ui_showdetailwidget.h"
#include "showdetailwidget.h"

ShowDetailWidget::ShowDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDetailWidget)
{
    ui->setupUi(this);

    subtitlesModel = new SubtitlesModel(this);
	ui->treeView->setModel(subtitlesModel);
	ui->treeView->header()->resizeSection(0, 200);
}

void ShowDetailWidget::init(const QString &showId, int season)
{
	_showId = showId;
	_season = season;
	subtitlesModel->init(_showId, _season);
}

void ShowDetailWidget::loadSubtitles()
{
//	subtitlesModel->init(_showId, _season);
}
