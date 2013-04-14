#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QQueue>
#include <QPair>

// ugly!
// TODO: hide all the internal part
class Request
{
public:
    Request() :
        ticketId(-1) {}

    Request(int ticketId, QNetworkReply *reply) {
        this->ticketId = ticketId;
        this->reply = reply;
    }

    bool isValid() const { return ticketId >= 0; }

    int ticketId;
    QNetworkReply *reply;
    QByteArray response;
};

class RequestManager : public QObject
{
    Q_OBJECT
public:
    static RequestManager &instance();

    int showsSearch(const QString &expression);
    int showsEpisodes(const QString &url, int season = -1, int episode = -1, bool summary = true, bool hide_notes = true);

signals:
    void requestFinished(int ticketId, const QByteArray &reponse);

public slots:

private slots:
    void httpFinished();
    void httpReadyRead();
    void httpError(QNetworkReply::NetworkError);

private:
    static RequestManager *_instance;
    QNetworkAccessManager network;
    int ticketId;
    QList<Request> currentRequests;
    QQueue<QPair<int, QNetworkRequest> > waitingQueue;

    explicit RequestManager(QObject *parent = 0);

    /*! \brief push the request in the waiting queue and returns a ticket id */
    int pushRequest(const QNetworkRequest &request);
    void animate();
    Request getRequest(QNetworkReply *reply, int *index = 0);
};

#endif // REQUESTMANAGER_H
