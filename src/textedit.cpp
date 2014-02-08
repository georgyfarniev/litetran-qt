#include "textedit.h"
#include <QApplication>
#include <QClipboard>

TextEdit::TextEdit(QWidget *parent) :
    QTextEdit(parent),
    clipboard(qApp->clipboard())
{
}

void TextEdit::copyAll()
{
    clipboard->setText(toPlainText());
}
