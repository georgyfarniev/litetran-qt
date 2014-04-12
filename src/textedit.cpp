#include "textedit.h"
#include <QApplication>
#include <QClipboard>

TextEdit::TextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void TextEdit::copyAll()
{
    qApp->clipboard()->setText(toPlainText());
}
