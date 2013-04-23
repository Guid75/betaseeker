#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class LoadingWidget;
}

// ugly! this class should be private, but I can't for now because Q_OBJECT need to be collected by moc
class LoadingEventManager : public QObject
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class LoadingWidget : public QWidget
{
    Q_OBJECT

    friend class LoadingEventManager;
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

    void computePosition();
    
};

#endif // LOADINGWIDGET_H
