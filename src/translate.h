#pragma once

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>
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
    void requestDictionary(const QString &sl, const QString &tl, const QString &text);
    void setDictionaryKey(const QString &dictionaryKey);

signals:
    void error(const QString &description);
    void languagesArrived(const LanguageVector &langs);
    void translationArrived(const QStringList &results);
	void detectionArrived(const QString &lang);
    void dictionaryArrived(const QString &result);
private:
	QNetworkAccessManager *mNetworkManager;
	QString mApiKey;
    QString mDictionaryKey;
};
