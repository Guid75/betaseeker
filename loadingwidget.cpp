#include "loadingwidget.h"
#include "ui_loadingwidget.h"

LoadingWidget::LoadingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadingWidget),
    labelState(0)
{
    ui->setupUi(this);

    startTimer(800);
}

void LoadingWidget::showLoadingMask(QWidget *widget)
{
}

void LoadingWidget::hideLoadingMask(QWidget *widget)
{
}

void LoadingWidget::timerEvent(QTimerEvent *)
{
    switch (labelState) {
    case 0:
        ui->labelMain->setText(tr("Fetching some data, please wait"));
        break;
    case 1:
        ui->labelMain->setText(tr("Fetching some data, please wait."));
        break;
    case 2:
        ui->labelMain->setText(tr("Fetching some data, please wait.."));
        break;
    case 3:
        ui->labelMain->setText(tr("Fetching some data, please wait..."));
        break;
    default:
        break;
    }
    labelState = (labelState + 1) % 4;
}
