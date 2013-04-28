#include <QLabel>
#include <QVBoxLayout>

#include "ui_subtitlewidget.h"
#include "subtitlewidget.h"

SubtitleWidget::SubtitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubtitleWidget)
{
    ui->setupUi(this);

    ui->label->installEventFilter(this);
}

void SubtitleWidget::setRecord(const QSqlRecord &record)
{
    _record = record;

    ui->label->setText(QString("[%1] %2").arg(record.value("quality").toInt()).arg(record.value("file").toString()));
}
