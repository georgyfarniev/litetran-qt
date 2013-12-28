#pragma once

#include <QString>
#include <QObject>

#define TRANSLATOR_URL "http://mymemory.translated.net/api/get"

class Translate : public QObject
{
    Q_OBJECT
public:
    explicit Translate(QObject *parent = 0);

    QString translate(const QString &text, const QString &sl, const QString &tl) const;
};
