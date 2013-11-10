#ifndef EPISODEFINDER_H
#define EPISODEFINDER_H

#include <QObject>
#include <QStringList>

class EpisodeFinder : public QObject
{
    Q_OBJECT
public:
    explicit EpisodeFinder(QObject *parent = 0);

    /*! \brief try to guess all episode files relative to parameters */
    QStringList getEpisodes(const QString &seasonPath, int season = -1, int episode = -1) const;
    
signals:
    
public slots:
    
};

#endif // EPISODEFINDER_H
