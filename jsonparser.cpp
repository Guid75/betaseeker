#include "jsonparser.h"

JsonParser::JsonParser(const QByteArray &content)
    : valid(false)
{
    QJsonParseError error;
    doc = QJsonDocument::fromJson(content, &error);
    if (doc.isNull()) {
        qDebug("error while parsing json");
        qDebug("%s: %d", qPrintable(error.errorString()), error.offset);
        return;
    }

    if (!doc.isObject()) {
        // TODO raise error, we except an object
        return;
    }

    QJsonObject obj = doc.object();
    QJsonValue val = obj.value("root");
    if (!val.isObject()) {
        // TODO raise error, root must be an object
        return;
    }

    _root = val.toObject();
    valid = true;
}
