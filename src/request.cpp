#include "request.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>

Request::Request(QObject *parent) :
    QObject(parent)
{
}

QByteArray Request::GET(QUrl req)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkReply *reply  = manager.get(QNetworkRequest(req));

    loop.exec();
    Request::checkForError(reply);

    return reply->readAll();
}

QByteArray Request::POST(const QUrl &url, const QByteArray &data)
{
    QNetworkAccessManager manager;
    QEventLoop loop;

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    req.setRawHeader("Content-Length", QByteArray::number(data.size()));

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply  = manager.post(req, data);

    loop.exec();
    Request::checkForError(reply);

    return reply->readAll();
}

void Request::checkForError(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(0, "Network Error", reply->errorString());
    }
}
