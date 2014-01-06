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
    const QByteArray encoded_text = text.toHtmlEscaped().toUtf8().toPercentEncoding();
    const QString req = QString("client=json&text=%1&sl=%2&tl=%3").arg(encoded_text, sl, tl);
    const QUrl url = QUrl(TRANSLATOR_URL);
    const QString response = Request::POST(url, req.toUtf8());
    const QJsonObject root = QJsonDocument::fromJson(response.toUtf8()).object();
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
    const QByteArray encoded_text = sample.toHtmlEscaped().toUtf8().toPercentEncoding();
    const QString req = QString("client=json&text=%1&sl=%2").arg(encoded_text, "auto");
    const QUrl url = QUrl(TRANSLATOR_URL);
    const QString response = Request::POST(url, req.toUtf8());
    const QJsonObject root = QJsonDocument::fromJson(response.toUtf8()).object();
    return root.value("src").toString();

}
