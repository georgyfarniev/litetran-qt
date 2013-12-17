#include "languagedb.h"
#include <QFile>
#include <QTextStream>

LanguageDB::LanguageDB(QObject *parent) :
    QObject(parent)
{
    QFile file(":/languages.csv");
    if(!file.open(QFile::ReadOnly))
        qFatal("Unable to open global language database! ");
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString str = in.readLine();
        QStringList result = str.split(",");
        lang_map[result.last()] = result.first();
    }
    file.close();
}
