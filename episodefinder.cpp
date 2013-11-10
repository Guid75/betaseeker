#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QMimeDatabase>
#include <QMimeType>
#include <QRegularExpression>

#include "episodefinder.h"

EpisodeFinder::EpisodeFinder(QObject *parent) :
    QObject(parent)
{
}

QStringList EpisodeFinder::getEpisodes(const QString &seasonPath, int season, int episode) const
{
    QDir seasonDir(seasonPath);
    QMimeDatabase db;
    QStringList files;
    foreach (const QFileInfo &fileInfo, seasonDir.entryInfoList()) {
        if (!fileInfo.isFile())
            continue;
        QMimeType mimeType = db.mimeTypeForFile(fileInfo);

        // must have got a video mime type
        if (!mimeType.name().startsWith("video/"))
            continue;

        if (season == -1) {
            files << fileInfo.absoluteFilePath();
            continue;
        }

        QString fileName = fileInfo.fileName();
        int fileSeason, fileEpisode;

        // SXXEXX expression?
        QRegularExpression re("\\bS([0-9]+)E([0-9]+)", QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match = re.match(fileName);
        if (match.hasMatch()) {
            fileSeason = match.captured(1).toInt();
            fileEpisode = match.captured(2).toInt();
            if (fileSeason == season && fileEpisode == episode)
                files << fileInfo.absoluteFilePath();
            continue;
        }

        // SxE expression?
        re.setPattern("\\b([0-9]+)x([0-9]+)");

        match = re.match(fileName);
        if (match.hasMatch()) {
            fileSeason = match.captured(1).toInt();
            fileEpisode = match.captured(2).toInt();
            if (fileSeason == season && fileEpisode == episode)
                files << fileInfo.absoluteFilePath();
            continue;
        }

    }
    return files;
}
