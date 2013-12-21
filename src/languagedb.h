#pragma once

#include <QObject>
#include <QList>
#include <QPair>

typedef QPair<QString, QString> Language;
typedef QList<Language> LanguageList;



class LanguageDB : public QObject
{
    Q_OBJECT
public:
    explicit LanguageDB(QObject *parent = 0);
    inline LanguageList dump() const {return langs;}
private:
    LanguageList langs;
};
