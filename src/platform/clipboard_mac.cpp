#include "clipboard.h"
#include "defines.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFile>

Clipboard::Clipboard(QObject *parent) :
    QObject(parent)
{
}

QString Clipboard::selectedText() const {
    QFile file(APP_OSX_TEXTFILE);
    if(!file.open(QFile::ReadOnly | QFile::Text))
        qWarning() << "Cannot open temp file at " << APP_OSX_TEXTFILE;

    text = file.readAll();
    file.close();
    if(!file.remove())
        qWarning() << "Cannot remove temp file at " << APP_OSX_TEXTFILE;
    return text;
}
