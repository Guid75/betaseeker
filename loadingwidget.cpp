#include "loadingwidget.h"
#include "ui_loadingwidget.h"

static QMap<QWidget*,LoadingWidget*> loadingWidgets;
static LoadingEventManager *eventManager = new LoadingEventManager;

LoadingWidget::LoadingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadingWidget),
    labelState(1)
{
    ui->setupUi(this);

    startTimer(800);
}

void LoadingWidget::showLoadingMask(QWidget *widget)
{
    if (loadingWidgets.find(widget) != loadingWidgets.end())
        return; // already shown

    LoadingWidget *loadingWidget = new LoadingWidget(widget);
    loadingWidget->computePosition();
    loadingWidget->show();

    widget->installEventFilter(eventManager);
    loadingWidgets.insert(widget, loadingWidget);
}

void LoadingWidget::hideLoadingMask(QWidget *widget)
{
    QMap<QWidget*,LoadingWidget*>::iterator loadingWidget = loadingWidgets.find(widget);
    if (loadingWidget == loadingWidgets.end())
        return; // unknown widget

    delete (*loadingWidget);

    widget->removeEventFilter(eventManager);
    loadingWidgets.remove(widget);
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

void LoadingWidget::computePosition()
{
    QWidget *widget = qobject_cast<QWidget*>(parent());

    move(widget->width() / 2 - width() / 2, widget->height() / 2 - height() / 2);
}


bool LoadingEventManager::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget*>(obj);
    QMap<QWidget*,LoadingWidget*>::iterator loadingWidget = loadingWidgets.find(widget);

    if (loadingWidget != loadingWidgets.end()) {
        switch (event->type()) {
        case QEvent::Resize:
            (*loadingWidget)->computePosition();
            break;
        case QEvent::Destroy:
            LoadingWidget::hideLoadingMask(widget);
            break;
        default:
            break;
        }
    }
    return QObject::eventFilter(obj, event);
}
