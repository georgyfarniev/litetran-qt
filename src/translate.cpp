#include "translate.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTextDocumentFragment>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLocale>

namespace constants
{
    const QString translate_api = "https://translate.yandex.net/api/v1.5/tr.json";
    const QString dictionary_api = "https://dictionary.yandex.net/api/v1/dicservice.json/lookup";
}

class TranslateParser
{
public:
	static LanguageVector parseLanguages(const QByteArray &data)
	{
		const QJsonObject root = QJsonDocument::fromJson(data).object();
		const QJsonArray dirs = root.value("dirs").toArray();
		const QJsonObject langs = root.value("langs").toObject();
		const QStringList keys = langs.keys();
		LanguageVector vec;

		for(const QString &key : keys)
			vec << Language(key, langs.value(key).toString());

		return vec;
	}

	static QString parseTranslate(const QByteArray &data)
	{
		return QJsonDocument::fromJson(data).object().value("text").toArray().first().toString();
	}

	static QString parseDetection(const QByteArray &data)
	{
		return QJsonDocument::fromJson(data).object().value("lang").toString();
	}
};

class DictionaryParser
{
public:
    static QString parseDictionary(const QByteArray &data)
    {
        const QJsonObject root = QJsonDocument::fromJson(data).object();
        const QJsonArray def = root.value("def").toArray();

        QString text;

        foreach(const QJsonValue val, def)
            text += parseDef(val.toObject());

        return text;
    }

    static QString parseDef(const QJsonObject &obj)
    {
        QString text;

        text += QString("<b>%1</b> <i>(%2</i>)<br>").
                arg(obj.value("text").toString()).
                arg(obj.value("pos").toString());

        const QJsonArray tr = obj.value("tr").toArray();
        foreach(const QJsonValue val, tr)
            text += parseTr(val.toObject());

        return text;
    }

    static QString parseTr(const QJsonObject &obj)
    {
        QString text;

        text += QString("&nbsp;").repeated(4);
        text += obj.value("text").toString();

        const QJsonArray syn = obj.value("syn").toArray();

        foreach(const QJsonValue val, syn)
            text += ", " + val.toObject().value("text").toString();


        text += "<br>";

        return text;
    }
};

TranslateEngine::TranslateEngine(QObject *parent)
	:QObject(parent), mNetworkManager(new QNetworkAccessManager(this))
{}

void TranslateEngine::setTranslateKey(const QString &key) {mApiKey = key;}

void TranslateEngine::requestLanguages()
{
    QUrl url(QString("%1/getLangs?key=%2&ui=%3").arg(constants::translate_api).arg(mApiKey).arg(QLocale::system().name().split('_').first()));
	QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::finished, [=](){
		const QByteArray result = reply->readAll();
		emit languagesArrived(TranslateParser::parseLanguages(result));
		reply->deleteLater();
	});
}

void TranslateEngine::requestTranslation(const QString &sl, const QString &tl, const QString &text)
{
    QUrl url(QString("%1/translate?key=%2&text=%3&lang=%4-%5").arg(constants::translate_api).arg(mApiKey).arg(text).arg(sl).arg(tl));
	QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::finished, [=](){
		const QByteArray result = reply->readAll();
		emit translationArrived(TranslateParser::parseTranslate(result));
		reply->deleteLater();
	});
}

void TranslateEngine::requestDetect(const QString &text, const QStringList &hint)
{
    QUrl url(QString("%1/detect?key=%2&text=%3&hint=%4").arg(constants::translate_api).arg(mApiKey).arg(text).arg(hint.join(',')));
	QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::finished, [=](){
		const QByteArray result = reply->readAll();
		emit detectionArrived(TranslateParser::parseDetection(result));
		reply->deleteLater();
	});
}

void TranslateEngine::requestDictionary(const QString &sl, const QString &tl, const QString &text)
{
    QUrl url(QString("%1?key=%2&lang=%3-%4&text=%5").arg(constants::dictionary_api).arg(mDictionaryKey).arg(sl).arg(tl).arg(text));
    QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::finished, [=](){
        const QByteArray result = reply->readAll();
        emit dictionaryArrived(DictionaryParser::parseDictionary(result));
        reply->deleteLater();
    });
}

void TranslateEngine::setDictionaryKey(const QString &dictionaryKey)
{
    mDictionaryKey = dictionaryKey;
}
