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
}

void ShowDetailWidget::loadSubtitles()
{
    qDebug("load subtitles");
}
