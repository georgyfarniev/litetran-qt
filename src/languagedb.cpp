#include "languagedb.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

LanguageDB::LanguageDB(QObject *parent) :
    QObject(parent)
{
    QFile file(":/languages.csv");
    if(!file.open(QFile::ReadOnly | QFile::Text))
        qFatal("Cannot read language database!");

    QTextStream in(&file);
    unsigned int counter = 0;
    while (!in.atEnd()) {
        const QString line = in.readLine();

        if(line.isEmpty()) {
            qWarning() << "Skip empty line at " << counter;
            continue;
        }

        QStringList result = line.split(",");

        if(result.size() != 2 )
            qFatal("Database error: invalid entry in line %d", counter);

        const QString code = result.first();
        const QString name = result.last();

        if(code.isEmpty() || name.isEmpty())
            qFatal("Database error: invalid value in line %d", counter);

        langs.append(Language(name, code));
        counter++;
    }
    file.close();
}
