#include <QFile>
#include <QUrl>
#include <QDir>

#include "requestmanager.h"

#include "downloadmanager.h"

class Download
{
public:
    QString filePath;
    QFile *file;
    QVariant userData;
};

DownloadManager *DownloadManager::_instance = 0;

DownloadManager &DownloadManager::instance()
{
    if (!_instance)
        _instance = new DownloadManager;

    return *_instance;
}

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    connect(&RequestManager::instance(), &RequestManager::requestReadyRead,
            this, &DownloadManager::requestReadyRead);
    connect(&RequestManager::instance(), &RequestManager::requestFinished,
            this, &DownloadManager::requestFinished);
}

int DownloadManager::download(const QString &file, const QString &url, const QString &dirPath, const QVariant &userData)
{
    int ticket = RequestManager::instance().pushRequest(QNetworkRequest(url));
    if (ticket == -1)
        return ticket;

    Download *download = new Download;
    downloads.insert(ticket, download);
    QString filePath = QDir(dirPath).filePath(file);
    download->file = new QFile(filePath);
    download->filePath = filePath;
    download->userData = userData;
    if (!download->file->open(QIODevice::WriteOnly)) {
        qCritical("Error while opening %s", qPrintable(filePath));
        // TODO manage the file opening error
    }
	return ticket;
}

void DownloadManager::requestReadyRead(int ticketId, const QByteArray &response)
{
    Download *download = downloads[ticketId];
    if (!download)
        return; // not for me

    if (!download->file->isOpen()) {
        return;
    }
    download->file->write(response);
}

void DownloadManager::requestFinished(int ticketId)
{
    Download *download = downloads[ticketId];
    if (!download)
        return; // not for me

    download->file->close();
    emit downloadFinished(ticketId, download->filePath, download->userData);
    downloads.remove(ticketId);
    delete download;
}

