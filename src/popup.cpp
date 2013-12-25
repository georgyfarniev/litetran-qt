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
    timer.setInterval(POPUP_HIDE_TIMEOUT);
    connect(clipboard, SIGNAL(selectionChanged()), this, SLOT(updatePosition()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(hidePopup()));
}

void Popup::show(const QString &text)
{
    const QString display_text = formatText(text);

    const int timeout = display_text.split(" ").size();

    timer.setInterval(POPUP_HIDE_TIMEOUT + (timeout * 1000));
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
    QString _text;
    QFontMetrics fm(QToolTip::font());
    QStringList lines = text.split(QChar('\n'));
    foreach (const QString &line_, lines) {
        QStringList words = line_.simplified().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (words.isEmpty())
            continue;

        QString line;
        foreach (const QString &word, words) {
            QString tmp = line;
            if (!tmp.isEmpty())
                tmp += " ";
            tmp += word;
            if (fm.width(tmp) >= POPUP_WIDTH) {
                if (!line.isEmpty()) {
                    if (!_text.isEmpty())
                        _text += "\n";
                    _text += line;
                    line = word;
                    continue;
                }
            }
            line = tmp;
        }
        if (!line.isEmpty()) {
            if (!_text.isEmpty())
                _text += "\n";
            _text += line;
        }
    }
    return _text;
}
