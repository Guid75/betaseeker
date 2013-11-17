#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QMimeDatabase>
#include <QMimeType>
#include <QRegularExpression>

#include "episodefinder.h"

/* some knowns subtitle extensions ( http://en.wikipedia.org/wiki/Subtitle_(captioning) ) */
static QStringList subtitleExtensions = QStringList()
        << "aqt"
        << "jss"
        << "sub"
        << "ttxt"
        << "pjs"
        << "psb"
        << "rt"
        << "smi"
        << "ssf"
        << "srt"
        << "gsub"
        << "usb"
        << "idx"
        << "stl";

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

QStringList EpisodeFinder::getSubtitles(const QString &seasonPath, int season, int episode) const
{
    QDir seasonDir(seasonPath);
    QMimeDatabase db;
    QStringList files;
    foreach (const QFileInfo &fileInfo, seasonDir.entryInfoList()) {
        if (!fileInfo.isFile())
            continue;

        // must be a subtitle
        if (subtitleExtensions.indexOf(fileInfo.suffix().toLower()) < 0)
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
            if (fileSeason == season && (fileEpisode == episode || episode == -1))
                files << fileInfo.absoluteFilePath();
            continue;
        }

        // SxE expression?
        re.setPattern("\\b([0-9]+)x([0-9]+)");

        match = re.match(fileName);
        if (match.hasMatch()) {
            fileSeason = match.captured(1).toInt();
            fileEpisode = match.captured(2).toInt();
            if (fileSeason == season && (fileEpisode == episode || episode == -1))
                files << fileInfo.absoluteFilePath();
            continue;
        }

    }
    return files;
}

