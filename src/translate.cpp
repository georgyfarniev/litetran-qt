#include "translate.h"
#include "request.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

Translate::Translate(QObject *parent)
    :QObject(parent)
{
}


QString Translate::translate(const QString &text, const QString &sl, const QString &tl) const
{
    const QUrl url("http://www.translate.google.com/translate_a/t");
    const QString req = QString("client=t&text=%1&sl=%2&tl=%3").arg(text, sl, tl);


    qDebug() << "FULL URI: " << url.toString() + req;

     QString ret = Request::POST(url, req);

    //it's a json array with trailing commas

     while(ret.contains(",,"))
         ret = ret.replace(",,", ",");

// Dirty hack. Just for fun.
     ret = ret.split("\",").first();
     ret = ret.replace(0, 4, "");
//     ret = ret.remove(ret.size() - 1, 1);

    qDebug() << "DOCUMENT IS NULL?: " << QJsonDocument::fromJson(ret.toUtf8()).isNull();

    return ret;
}
