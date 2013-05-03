#include "requestmanager.h"

#include "commandmanager.h"

static const QString apiKey = "9adb4ab628c6";
static const QString websiteUrl = "http://api.betaseries.com";

class Command
{
public:

    QByteArray response;
};

CommandManager::CommandManager(QObject *parent) :
    QObject(parent)
{
    connect(&RequestManager::instance(), &RequestManager::requestReadyRead,
            this, &CommandManager::requestReadyRead);
    connect(&RequestManager::instance(), &RequestManager::requestFinished,
            this, &CommandManager::requestFinished);
}

int CommandManager::pushCommand(const QString &url)
{
    int ticket = RequestManager::instance().pushRequest(QNetworkRequest(url));
    // TODO test if ticket is not -1

    Command *command = new Command;
    commands.insert(ticket, command);

    return ticket;
}

void CommandManager::requestReadyRead(int ticketId, const QByteArray &response)
{
    Command *command = commands[ticketId];
    if (!command)
        return; // not for me

    command->response.append(response);
}

CommandManager *CommandManager::_instance = 0;

CommandManager &CommandManager::instance()
{
    if (!_instance)
        _instance = new CommandManager;

    return *_instance;
}

int CommandManager::showsSearch(const QString &expression)
{
    return pushCommand(QString("%1/shows/search.json?title=%2&key=%3").arg(websiteUrl).arg(expression).arg(apiKey));
}

int CommandManager::showsEpisodes(const QString &url, int season, int episode, bool summary, bool hide_notes)
{
    QString str = QString("%1/shows/episodes/%2.json?&key=%3").arg(websiteUrl).arg(url).arg(apiKey);

    if (season >= 0)
        str.append(QString("&season=%1").arg(season));
    if (episode >= 0)
        str.append(QString("&episode=%1").arg(episode));
    if (summary)
        str.append("&summary=1");
    if (hide_notes)
        str.append("hide_notes=1");

    return pushCommand(str);
}

int CommandManager::subtitlesShow(const QString &showId, int season, int episode, const QString &language)
{
    QString str = QString("%1/subtitles/show/%2.json?&key=%3").arg(websiteUrl).arg(showId).arg(apiKey);
    if (season >= 0)
        str.append(QString("&season=%1").arg(season));
    if (episode >= 0)
        str.append(QString("&episode=%1").arg(episode));
    if (!language.isEmpty())
        str.append(QString("&language=%1").arg(language));

    return pushCommand(str);
}

int CommandManager::subtitlesShowByFile(const QString &showId, const QString &fileName, const QString &language)
{
    QString str = QString("%1/subtitles/show/%2.json?&key=%3&file=%4").arg(websiteUrl).arg(showId).arg(apiKey).arg(fileName);
    if (!language.isEmpty())
        str.append(QString("&language=%1").arg(language));

    return pushCommand(str);
}


void CommandManager::requestFinished(int ticketId)
{
    Command *command = commands[ticketId];
    if (!command)
        return; // not for me

    emit commandFinished(ticketId, command->response);
    commands.remove(ticketId);
    delete command;
}
