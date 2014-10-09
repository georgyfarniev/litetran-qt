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
    return QApplication::clipboard()->text(QClipboard::Selection);
}
