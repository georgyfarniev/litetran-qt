#include "translate.h"
#include "networkmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTextDocumentFragment>

#define TRANSLATOR_URL "http://www.translate.google.com/translate_a/t"

Translate::Translate(QObject *parent)
    :QObject(parent),
     enable_dict(false),
     network_manager(NetworkManager::instance())
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
        result += "<hr>";
        foreach(const QJsonValue &val, dict) {
            const QString pos = val.toObject().value("pos").toString();
            result += QString("<i><b>%1</b> - %2</i>").arg(text, pos);
            const QJsonArray entries = val.toObject().value("entry").toArray();
            foreach(const QJsonValue &entry, entries) {
                const QString word = entry.toObject().value("word").toString();
                const QStringList reverse_translations = entry.toObject().value("reverse_translation").toVariant().toStringList();
                result +=  QString("<br><b>%1</b> - %2").arg(word, reverse_translations.join(", "));
            }
            if(val != dict.last())
                result += "<br><br>";
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
    const QString response = network_manager->POST(url, req.toUtf8());

    return QJsonDocument::fromJson(response.toUtf8());
}
