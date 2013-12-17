#include "popup.h"

#include <QClipboard>
#include <QToolTip>
#include <QApplication>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

static const int tooltip_width = 640; // 640px ought to be enough for anybody ;)
// TODO auto-detect active screen width.

static const int tooltip_hide_timeout = 5000; // 5sec
// TODO move to Settings dialog.

Popup::Popup(QObject *parent) :
    QObject(parent),
    m_clipboard(qApp->clipboard()),
    m_cursor_pos_locked(false),
    m_hide_timer(this)
{
    m_hide_timer.setSingleShot(true);
    connect(m_clipboard, SIGNAL(selectionChanged()), this, SLOT(updateCursorPos()));
    connect(&m_hide_timer, SIGNAL(timeout()), this, SLOT(slotHideToolTip()));
}


void Popup::show(const QString &text_){
    QString text;
    QFontMetrics fm(QToolTip::font());
    QStringList lines = text_.split(QChar('\n'));
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
            if (fm.width(tmp) >= tooltip_width) {
                if (!line.isEmpty()) {
                    if (!text.isEmpty())
                        text += "\n";
                    text += line;
                    line = word;
                    continue;
                }
            }
            line = tmp;
        }
        if (!line.isEmpty()) {
            if (!text.isEmpty())
                text += "\n";
            text += line;
        }
    }
    if (text.isEmpty())
        return;

    QToolTip::showText(m_cursor_pos,text);
    if (tooltip_hide_timeout)
        m_hide_timer.start(tooltip_hide_timeout);
}

void Popup::setLocked(bool b){
    m_cursor_pos_locked = b;
}

void Popup::updateCursorPos(){
    if(!m_cursor_pos_locked)
        m_cursor_pos = QCursor::pos();
}

void Popup::slotHideToolTip() {
    QToolTip::hideText();
}

