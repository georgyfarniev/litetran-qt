#include "translate.h"
#include "request.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextDocumentFragment>


Translate::Translate(QObject *parent)
    :QObject(parent)
{
}

QString Translate::translate(const QString &text, const QString &sl, const QString &tl) const
{
    QByteArray html = text.toUtf8();

    html.replace("&", "&amp;");
    html.replace("<", "&lt;");
    html.replace(">", "&gt;");
    html.replace("\n", "<br>");

    QByteArray query = "v=1.0&format=html";
    query += "&langpair=" + sl.toLatin1() + "%7C" + tl.toLatin1();
    query += "&q=" + html.toPercentEncoding();

    QUrl url(TRANSLATOR_URL);

    const QString  rawdata = Request::POST(url, query);

    QJsonObject obj = QJsonDocument::fromJson(rawdata.toUtf8()).object();
    QString res = obj.value("responseData").toObject().value("translatedText").toString();

    return QTextDocumentFragment::fromHtml(res).toPlainText();
}

