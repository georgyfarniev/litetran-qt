#include "networkmanager.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QMutex>

NetworkManager::NetworkManager():
    qt_network_manager(new QNetworkAccessManager),
    m_event_loop(new QEventLoop)
{
    QObject::connect(qt_network_manager, &QNetworkAccessManager::finished, m_event_loop, &QEventLoop::quit);
}

NetworkManager *NetworkManager::instance()
{
    static QMutex mutex;
    static NetworkManager *singleton = 0;

    if(!singleton) {
        mutex.lock();
        singleton =  new NetworkManager;
        mutex.unlock();
    }
    return singleton;
}

QByteArray NetworkManager::GET(QUrl req)
{
    QNetworkReply *reply  = qt_network_manager->get(QNetworkRequest(req));
    m_event_loop->exec();
    checkForError(reply);
    return reply->readAll();
}

QByteArray NetworkManager::POST(const QUrl &url, const QByteArray &data)
{
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    req.setRawHeader("Content-Length", QByteArray::number(data.size()));

    QNetworkReply *reply  = qt_network_manager->post(req, data);
    m_event_loop->exec();
    checkForError(reply);

    return reply->readAll();
}

void NetworkManager::checkForError(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(0, "Network Error", reply->errorString());
    }
}
