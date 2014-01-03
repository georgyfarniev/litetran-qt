#include "popup.h"
#include <QToolTip>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

Popup::Popup(QObject *parent) :
    QObject(parent),
    cursor_locked(false)
{
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(hidePopup()));
}

void Popup::show(const QString &text)
{
    const QString display_text = formatText(text);
    const int words_count = display_text.split(" ").size();

    if (display_text.isEmpty())
        return;

    QToolTip::showText(cursor_pos, display_text);
    timer.start(POPUP_MIN_TIMEOUT + (words_count * 1000));
}

void Popup::freezeCursorPosition()
{
    cursor_pos = QCursor::pos();
}

void Popup::hidePopup()
{
    QToolTip::hideText();
}

QString Popup::formatText(const QString &text) const
{
    QFontMetrics fm(QToolTip::font());
    QStringList words = text.simplified().split(QRegExp("\\s+"), QString::SkipEmptyParts);

    QString outstr;
    QString line;

    foreach(const QString &word, words) {
        if(!line.isEmpty())
            line += " ";
        line += word;

        if(fm.width(line) >= POPUP_WIDTH || word == words.last()) {
            outstr += line +  "\n";
            line.clear();
        }
    }

    return outstr.trimmed();
}
