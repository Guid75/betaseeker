#ifndef SHOWDETAILWIDGET_H
#define SHOWDETAILWIDGET_H

#include <QWidget>

class SubtitlesModel;
namespace Ui {
class ShowDetailWidget;
}

class ShowDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowDetailWidget(QWidget *parent = 0);

    void loadSubtitles();
    
signals:
    
public slots:

private:
    Ui::ShowDetailWidget *ui;
    int subtitleTicket;
    SubtitlesModel *subtitlesModel;
};

#endif // SHOWDETAILWIDGET_H
