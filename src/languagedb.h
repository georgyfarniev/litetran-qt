#pragma once

#include <QObject>
#include <QMap>

typedef QMap<QString, QString> LanguageMap;


class LanguageDB : public QObject
{
    Q_OBJECT
public:
    explicit LanguageDB(QObject *parent = 0);


    inline LanguageMap dump() const {return lang_map;}
private:
    LanguageMap lang_map;
};
