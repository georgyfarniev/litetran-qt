#pragma once

#include <QUrl>
#include <QByteArray>

class QNetworkReply;

class Request
{
public:
    static QByteArray GET(QUrl req);
    static QByteArray POST(const QUrl &url, const QByteArray &data);
private:
    static void checkForError(QNetworkReply *reply);
};
