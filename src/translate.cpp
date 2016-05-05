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
    const QString url_getlangs = "https://translate.yandex.net/api/v1.5/tr.json/getLangs";
	const QString url_detect = "https://translate.yandex.net/api/v1.5/tr.json/detect";
	const QString url_translate = "https://translate.yandex.net/api/v1.5/tr.json/translate";
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

TranslateEngine::TranslateEngine(QObject *parent)
	:QObject(parent), mNetworkManager(new QNetworkAccessManager(this))
{}

void TranslateEngine::setTranslateKey(const QString &key) {mApiKey = key;}

void TranslateEngine::requestLanguages()
{
	QUrl url(QString("%1?key=%2&ui=%3").arg(constants::url_getlangs).arg(mApiKey).arg(QLocale::system().name().split('_').first()));
	QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::finished, [=](){
		const QByteArray result = reply->readAll();
		emit languagesArrived(TranslateParser::parseLanguages(result));
		reply->deleteLater();
	});
}

void TranslateEngine::requestTranslation(const QString &sl, const QString &tl, const QString &text)
{
	QUrl url(QString("%1?key=%2&text=%3&lang=%4-%5").arg(constants::url_translate).arg(mApiKey).arg(text).arg(sl).arg(tl));
	QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::finished, [=](){
		const QByteArray result = reply->readAll();
		emit translationArrived(TranslateParser::parseTranslate(result));
		reply->deleteLater();
	});
}

void TranslateEngine::requestDetect(const QString &text, const QStringList &hint)
{
	QUrl url(QString("%1?key=%2&text=%3&hint=%4").arg(constants::url_detect).arg(mApiKey).arg(text).arg(hint.join(',')));
	QNetworkReply *reply  = mNetworkManager->get(QNetworkRequest(url));

	connect(reply, &QNetworkReply::finished, [=](){
		const QByteArray result = reply->readAll();
		emit detectionArrived(TranslateParser::parseDetection(result));
		reply->deleteLater();
	});
}
