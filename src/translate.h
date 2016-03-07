#pragma once

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include "models.h"

class TranslateEngine : public QObject
{
	Q_OBJECT
public:
	TranslateEngine(QObject *parent = 0);
	void setTranslateKey(const QString &key);
	void requestLanguages();
	void requestTranslation(const QString &sl, const QString &tl, const QString &text);
	void requestDetect(const QString &text, const QStringList &hint);
signals:
	void error(const QString &description);
	void languagesArrived(const LanguageVector &langs);
	void translationArrived(const QString &result);
	void detectionArrived(const QString &lang);
private:
	QNetworkAccessManager *mNetworkManager;
	QString mApiKey;
};
