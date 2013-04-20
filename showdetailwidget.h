#ifndef SHOWDETAILWIDGET_H
#define SHOWDETAILWIDGET_H

#include <QWidget>

namespace Ui {
class ShowDetailWidget;
}

class ShowDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowDetailWidget(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    Ui::ShowDetailWidget *ui;
};

#endif // SHOWDETAILWIDGET_H
