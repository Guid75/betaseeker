#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QMap>
#include <QVariant>

class Download;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    static DownloadManager &instance();
    
    /*! \brief Schedule a file downloading
     */
    int download(const QString &fileName, const QString &url, const QString &dirPath, const QVariant &userData = QVariant());

signals:
    void downloadFinished(int ticketId, const QString &filePath, const QVariant &userData);

public slots:

private:
    static DownloadManager *_instance;
    QMap<int,Download*> downloads;

    explicit DownloadManager(QObject *parent = 0);

private slots:
    void requestReadyRead(int ticketId, const QByteArray &response);
    void requestFinished(int ticketId);
};

#endif // DOWNLOADMANAGER_H
