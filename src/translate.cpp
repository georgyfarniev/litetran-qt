#include "translate.h"
#include "networkmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTextDocumentFragment>

#define TRANSLATOR_URL "http://api.mymemory.translated.net/get"

Translate::Translate(QObject *parent)
    :QObject(parent),
     enable_dict(false),
     network_manager(NetworkManager::instance())
{
}

QString Translate::translate(const QString &text, const QString &sl, const QString &tl) const
{
	const QString params = QString(TRANSLATOR_URL"?q=%1&langpair=%2|%3").arg(text, sl, tl);
	const QByteArray encoded_text = text.toHtmlEscaped().toUtf8().toPercentEncoding();
	const QString req = params + encoded_text;
	const QString response = network_manager->GET(params);
	const QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
	return doc.object().value("responseData").toObject().value("translatedText").toString();
}

QJsonDocument Translate::query(const QString &params, const QString &text) const
{
    const QByteArray encoded_text = text.toHtmlEscaped().toUtf8().toPercentEncoding();
    const QString req = params + encoded_text;
    const QUrl url = QUrl(TRANSLATOR_URL);
    const QString response = network_manager->POST(url, req.toUtf8());

    return QJsonDocument::fromJson(response.toUtf8());
}
