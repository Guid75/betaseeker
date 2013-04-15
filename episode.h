#ifndef EPISODE_H
#define EPISODE_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Episode
{
public:
    Episode(int episode);

    int episode() const { return _episode; }
    const QString &title() const { return _title; }
    const QString &number() const { return _number; }
    int global() const { return _global; }
    const QDateTime &date() const { return _date; }
    int comments() const { return _comments; }

    void parseEpisode(const QJsonObject &root);

private:
    int _episode;
	QString _title;
    QString _number; // eg: S01E01
    int _global; // the global index of the episode
    QDateTime _date; // the release date
    int _comments;
};

#endif // EPISODE_H
