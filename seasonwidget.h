#ifndef SEASONWIDGET_H
#define SEASONWIDGET_H

#include <QWidget>
#include <QMap>

class EpisodeWidget;

class SeasonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SeasonWidget(QWidget *parent = 0);
    
    void init(const QString &showId, int season);

signals:
    
public slots:
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QString _showId;
    int _season;
    QMap<int,EpisodeWidget*> tickets;

    void clear();
    void parseSubtitles(EpisodeWidget *episodeWidget, const QByteArray &response);

private slots:
    void requestFinished(int ticketId, const QByteArray &response);
};

#endif // SUBTITLESWIDGET_H
