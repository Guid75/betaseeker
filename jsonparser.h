#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonDocument>

class JsonParser
{
public:
    JsonParser(const QByteArray &content);

    //JsonObject &root() { return _root; }
    const QJsonObject &root() const { return _root; }

    bool isValid() const { return valid; }

private:
    bool valid;
    QJsonDocument doc;
    QJsonObject _root;
};

#endif // JSONPARSER_H
