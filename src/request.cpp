#include "request.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

Request::Request(QObject *parent) :
    QObject(parent)
{
}

QByteArray Request::GET(QUrl req)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply  = manager.get(QNetworkRequest(req));

    loop.exec();

    return reply->readAll();
}


QByteArray Request::POST(const QUrl &url, const QString &data)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    req.setRawHeader("Content-Length", QByteArray::number(data.size()));

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply  = manager.post(req, data.toUtf8());

    loop.exec();

    return reply->readAll();
}
