#include "popup.h"
#include <QClipboard>
#include <QToolTip>
#include <QApplication>
#include <QDebug>
#include <QApplication>

Popup::Popup(QObject *parent) :
    QObject(parent),
    clipboard(qApp->clipboard()),
    cursor_locked(false)
{
    timer.setSingleShot(true);

    connect(clipboard, SIGNAL(selectionChanged()), this, SLOT(updatePosition()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(hidePopup()));
}

void Popup::show(const QString &text)
{
    const QString display_text = formatText(text);

    // calculate timeout which is depend of words count in text
    const int words_count = display_text.split(" ").size();
    timer.setInterval(POPUP_MIN_TIMEOUT + (words_count * 1000));
    if (display_text.isEmpty())
        return;

    QToolTip::showText(cursor_pos, display_text);
    timer.start();
}

void Popup::setLocked(bool b)
{
    cursor_locked = b;
}

void Popup::updatePosition()
{
    if(!cursor_locked)
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
