#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>

namespace Ui {
class LoadingWidget;
}

class LoadingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingWidget(QWidget *parent = 0);

    static void showLoadingMask(QWidget *widget);
    static void hideLoadingMask(QWidget *widget);

signals:
    
public slots:

protected:
    void timerEvent(QTimerEvent *);

private:
    Ui::LoadingWidget *ui;
    int labelState;
    
};

#endif // LOADINGWIDGET_H
