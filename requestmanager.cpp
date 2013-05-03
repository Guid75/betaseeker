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

#include <QCoreApplication>

#include "requestmanager.h"

RequestManager *RequestManager::_instance = 0;

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

    emit requestFinished(request.ticketId);

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

    emit requestReadyRead(request.ticketId, response);
/*    request.response.append(response);
    currentRequests[index] = request;*/
}
