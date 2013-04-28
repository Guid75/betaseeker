#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include <QWidget>
#include <QSqlRecord>

class QLabel;

namespace Ui {
class SubtitleWidget;
}

class SubtitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubtitleWidget(QWidget *parent = 0);

    const QSqlRecord &record() const { return _record; }
    void setRecord(const QSqlRecord &record);

signals:
    
public slots:

private:
    Ui::SubtitleWidget *ui;
    QSqlRecord _record;
    
};

#endif // SUBTITLEWIDGET_H
