#include "languagedb.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

LanguageDB::LanguageDB(QObject *parent) :
    QObject(parent)
{
    QFile file(":/languages.csv");
    if(!file.open(QFile::ReadOnly | QFile::Text))
        qFatal("Cannot read language database!");

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        const QStringList result = line.split(",", QString::SkipEmptyParts);
        if(result.size() != 2 )
            qFatal("Language database error: invalid entry");
        langs.append(Language(result.last(), result.first()));
    }
    file.close();
}
