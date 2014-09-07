#pragma once

#include <QObject>
#include <QUrl>
#include <QByteArray>

class QNetworkReply;

class Request : public QObject
{
    Q_OBJECT
public:
    explicit Request(QObject *parent = 0);
    static QByteArray GET(QUrl req);
    static QByteArray POST(const QUrl &url, const QByteArray &data);
private:
    static void checkForError(QNetworkReply *reply);
};
