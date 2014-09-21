#pragma once

#include <QString>
#include <QObject>

class NetworkManager;

class Translate : public QObject
{
    Q_OBJECT
public:
    explicit Translate(QObject *parent = 0);
    QString translate(const QString &text, const QString &sl, const QString &tl) const;
    QString detect(const QString &sample) const;
    inline void setDictionaryEnabled(bool enabled) {enable_dict = enabled;}
private:
    QJsonDocument query(const QString &params, const QString &text) const;
    bool enable_dict;
    NetworkManager *network_manager;
};
