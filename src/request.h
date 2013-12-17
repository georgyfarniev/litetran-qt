#ifndef REQUEST_H
#define REQUEST_H

#include <QObject>
#include <QUrl>
#include <QByteArray>

class Request : public QObject
{
    Q_OBJECT
public:
    explicit Request(QObject *parent = 0);

    static QByteArray GET(QUrl req);
    static QByteArray POST(const QUrl &url, const QString &data);
};

#endif // REQUEST_H
