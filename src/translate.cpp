#include "translate.h"
#include "request.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextDocumentFragment>

Translate::Translate(QObject *parent)
    :QObject(parent),
     enable_dict(true)
{
}

QString Translate::translate(const QString &text, const QString &sl, const QString &tl) const
{
    const QString params = QString("client=json&sl=%1&tl=%2&text=").arg(sl, tl);
    const QJsonObject root = query(params, text).object();
    const QJsonArray sentences = root.value("sentences").toArray();
    const QJsonArray dict = root.value("dict").toArray();

    QString result;
    foreach(const QJsonValue &val, sentences)
        result += val.toObject().value("trans").toString();

    if(enable_dict && !dict.isEmpty()) {
        foreach(const QJsonValue &val, dict) {
            const QString pos = val.toObject().value("pos").toString();
            result += QString("<br><br><i><b>%1</b> - %2</i><br>").arg(text, pos);
            const QJsonArray entries = val.toObject().value("entry").toArray();
            foreach(const QJsonValue &entry, entries) {
                const QString word = entry.toObject().value("word").toString();
                const QStringList reverse_translations = entry.toObject().value("reverse_translation").toVariant().toStringList();
                result +=  QString("<br><b>%1</b> - %2").arg(word, reverse_translations.join(", "));
            }
        }
    }
    return result;
}

QString Translate::detect(const QString &sample) const
{
    const QJsonObject root = query("client=json&sl=auto&text=", sample).object();
    return root.value("src").toString();
}

QJsonDocument Translate::query(const QString &params, const QString &text) const
{
    const QByteArray encoded_text = text.toHtmlEscaped().toUtf8().toPercentEncoding();
    const QString req = params + encoded_text;
    const QUrl url = QUrl(TRANSLATOR_URL);
    const QString response = Request::POST(url, req.toUtf8());

    return QJsonDocument::fromJson(response.toUtf8());
}
