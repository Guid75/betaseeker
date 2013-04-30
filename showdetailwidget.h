#ifndef SHOWDETAILWIDGET_H
#define SHOWDETAILWIDGET_H

#include <QWidget>

class SubtitlesModel;
class SeasonWidget;

namespace Ui {
class ShowDetailWidget;
}

class ShowDetailWidget : public QWidget
{
    Q_OBJECT
public:
	explicit ShowDetailWidget(QWidget *parent = 0);

	void init(const QString &showId, int season);

signals:

public slots:

private:
	QString _showId;
	int _season;
    Ui::ShowDetailWidget *ui;
    int subtitleTicket;
    SubtitlesModel *subtitlesModel;
    SeasonWidget *seasonWidget;

private slots:
    void on_pushButtonDefineIt_clicked();
    void on_pushButtonForgetIt_clicked();
};

#endif // SHOWDETAILWIDGET_H
