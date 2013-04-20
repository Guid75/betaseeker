#include "ui_showdetailwidget.h"
#include "showdetailwidget.h"

ShowDetailWidget::ShowDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDetailWidget)
{
    ui->setupUi(this);
}
