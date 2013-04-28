#ifndef EPISODEWIDGET_H
#define EPISODEWIDGET_H

#include <QWidget>
#include <QSqlRecord>

class QVBoxLayout;

namespace Ui {
class EpisodeWidget;
}

class EpisodeWidget : public QWidget
{
    Q_OBJECT
public:
    enum State {
        State_Waiting,
        State_Fetching,
        State_Fetched,
        State_Error
    };

    explicit EpisodeWidget(QWidget *parent = 0);

    void init(const QSqlRecord &record, const QString &showId, int season);

    int episode() const;

    State state() const { return _state; }
    void setState(State state) { _state = state; }

    void expand();
    void collapse();
    void toggleCollapse();
    
signals:
    
public slots:

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QString _showId;
    int _season;
    Ui::EpisodeWidget *ui;
    QSqlRecord _record;
    State _state;
    bool collapsed;
    QVBoxLayout *subtitlesLayout;
    int ticket;

    void parseSubtitles(const QByteArray &response);

private slots:
    void requestFinished(int ticketId, const QByteArray &response);
};

#endif // EPISODEWIDGET_H
