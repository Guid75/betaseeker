//  Copyright 2013 Guillaume Denry (guillaume.denry@gmail.com)
//  This file is part of BetaSeeker.
//
//  BetaSeeker is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  BetaSeeker is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with BetaSeeker.  If not, see <http://www.gnu.org/licenses/>.

#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
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
};

class RequestManager : public QObject
{
    Q_OBJECT
public:
    static RequestManager &instance();

    /*! \brief push the request in the waiting queue and returns a ticket id */
    int pushRequest(const QNetworkRequest &request);

signals:
    void requestReadyRead(int ticketId, const QByteArray &reponse);
    void requestFinished(int ticketId);

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

    void animate();
    Request getRequest(QNetworkReply *reply, int *index = 0);
};

#endif // REQUESTMANAGER_H
