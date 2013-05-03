#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QObject>
#include <QMap>

class Command;

class CommandManager : public QObject
{
    Q_OBJECT
public:
    static CommandManager &instance();

    int showsSearch(const QString &expression);
    int showsEpisodes(const QString &url, int season = -1, int episode = -1, bool summary = true, bool hide_notes = true);
    int subtitlesShow(const QString &showId, int season = -1, int episode = -1, const QString &language = QString());
    int subtitlesShowByFile(const QString &showId, const QString &fileName, const QString &language = QString());

signals:
    // TODO returns a JSON document and parse error in the CommandManager
    void commandFinished(int ticketId, const QByteArray &response);

public slots:

private:
    QMap<int,Command*> commands;

    static CommandManager *_instance;

    explicit CommandManager(QObject *parent = 0);
    int pushCommand(const QString &url);

private slots:
    void requestReadyRead(int ticketId, const QByteArray &response);
    void requestFinished(int ticketId);
};

#endif // COMMANDMANAGER_H
