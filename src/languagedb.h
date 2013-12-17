#ifndef LANGUAGEDB_H
#define LANGUAGEDB_H

#include <QObject>
#include <QMap>

typedef QMap<QString, QString> LanguageMap;


class LanguageDB : public QObject
{
    Q_OBJECT
public:
    explicit LanguageDB(QObject *parent = 0);


    inline LanguageMap languages() const {return lang_map;}
private:
    LanguageMap lang_map;
};

#endif // LANGUAGEDB_H
