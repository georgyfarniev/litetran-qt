#pragma once

#include <QUrl>
#include <QByteArray>

class QNetworkAccessManager;
class QNetworkReply;
class QEventLoop;

class NetworkManager
{
public:
    static  NetworkManager* instance();
    QByteArray GET(QUrl req);
    QByteArray POST(const QUrl &url, const QByteArray &data);
private:
    NetworkManager();
    NetworkManager(const NetworkManager& root);
    NetworkManager& operator=(const NetworkManager&);
    void checkForError(QNetworkReply *reply);
    QNetworkAccessManager *qt_network_manager;
    QEventLoop *m_event_loop;
};
