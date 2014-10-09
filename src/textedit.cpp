#include "textedit.h"
#include <QApplication>
#include <QClipboard>

TextEdit::TextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void TextEdit::copyAll()
{
    QApplication::clipboard()->setText(toPlainText());
}
