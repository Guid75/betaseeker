#include "episode.h"

Episode::Episode(int episode) :
    _episode(episode)
{
}

void Episode::parseEpisode(const QJsonObject &root)
{
    _title = root.value("title").toString();
    _number = root.value("number").toString();
    _global = root.value("global").toString().toInt();
    _date = QDateTime::fromMSecsSinceEpoch(root.value("date").toDouble() * 1000);
    _comments = root.value("comments").toString().toInt();
}
