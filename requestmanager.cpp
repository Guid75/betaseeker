#include <QCoreApplication>

#include "requestmanager.h"


//////////////////////

RequestManager *RequestManager::_instance = 0;

static const QString apiKey = "9adb4ab628c6";
static const QString websiteUrl = "http://api.betaseries.com";
static const int maxWaitingRequests = 100;
static const int maxLivingRequests = 3;

RequestManager::RequestManager(QObject *parent) :
    QObject(parent),
    ticketId(0)
{
}

RequestManager &RequestManager::instance()
{
    if (!_instance)
        _instance = new RequestManager();
    return *_instance;
}

int RequestManager::showsSearch(const QString &expression)
{
    QUrl url = QString("%1/shows/search.json?title=%2&key=%3").arg(websiteUrl).arg(expression).arg(apiKey);

    return pushRequest(QNetworkRequest(url));
}

int RequestManager::showsEpisodes(const QString &url, int season, int episode, bool summary, bool hide_notes)
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

    return pushRequest(QNetworkRequest(QUrl(str)));
}

int RequestManager::subtitlesShow(const QString &showId, int season, int episode, const QString &language)
{
    QString str = QString("%1/subtitles/show/%2.json?&key=%3").arg(websiteUrl).arg(showId).arg(apiKey);
    if (season >= 0)
        str.append(QString("&season=%1").arg(season));
    if (episode >= 0)
        str.append(QString("&episode=%1").arg(episode));
    if (!language.isEmpty())
        str.append(QString("&language=%1").arg(language));

    return pushRequest(QNetworkRequest(QUrl(str)));
}

int RequestManager::subtitlesShowByFile(const QString &showId, const QString &fileName, const QString &language)
{
    QString str = QString("%1/subtitles/show/%2.json?&key=%3&file=%4").arg(websiteUrl).arg(showId).arg(apiKey).arg(fileName);
    if (!language.isEmpty())
        str.append(QString("&language=%1").arg(language));

    return pushRequest(QNetworkRequest(QUrl(str)));
}

int RequestManager::pushRequest(const QNetworkRequest &request)
{
    int id;
    if (waitingQueue.count() >= maxWaitingRequests)
        return -1;

    waitingQueue << QPair<int,QNetworkRequest>(ticketId,request);
    id = ticketId;
    ticketId = (ticketId + 1) % maxWaitingRequests;

    animate();
    return id;
}

void RequestManager::animate()
{
    if (currentRequests.count() == maxLivingRequests)
        return; // no free place

    // feed entirely all free slots
    while (currentRequests.count() < maxLivingRequests) {
        if (waitingQueue.isEmpty())
            break;

        QPair<int,QNetworkRequest> head = waitingQueue.dequeue();
        int ticketId = head.first;
        QNetworkRequest request = head.second;
        request.setHeader(QNetworkRequest::UserAgentHeader, "betaseeker v" + QCoreApplication::instance()->applicationVersion());
        QNetworkReply *reply = network.get(request);
        currentRequests << Request(ticketId, reply);
        connect(reply, SIGNAL(finished()),
                this, SLOT(httpFinished()));
        connect(reply, SIGNAL(readyRead()),
                this, SLOT(httpReadyRead()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(httpError(QNetworkReply::NetworkError)));
    }
}

void RequestManager::httpError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qDebug("error: %s", qPrintable(reply->errorString()));
}

void RequestManager::httpFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    int index;

    Request request = getRequest(reply, &index);

    Q_ASSERT(request.isValid());

    emit requestFinished(request.ticketId, request.response);

    currentRequests.removeAt(index);

    reply->deleteLater();
    animate();
}

Request RequestManager::getRequest(QNetworkReply *reply, int *index)
{
    for (int i = 0; i < currentRequests.count(); i++)
    {
        Request request = currentRequests[i];
        if (request.reply == reply) {
            if (index)
                *index = i;
            return request;
        }
    }
    return Request();
}

void RequestManager::httpReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray response = reply->readAll();
    int index;

    Request request = getRequest(reply, &index);

    Q_ASSERT(request.isValid());

    request.response.append(response);
    currentRequests[index] = request;
}
