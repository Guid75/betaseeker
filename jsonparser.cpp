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
