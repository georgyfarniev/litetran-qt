#include "clipboard.h"
#include "defines.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFile>

#ifdef APP_WM_WINDOWS
#include <qt_windows.h>
#endif

Clipboard::Clipboard(QObject *parent) :
    QObject(parent),
    clipboard(qApp->clipboard())
{
}

QString Clipboard::selectedText() const {
    QString text;
#if defined(APP_WM_X11)
    text = clipboard->text(QClipboard::Selection);
#elif defined(APP_WM_WINDOWS)
    Sleep(200);
    keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_EXTENDEDKEY | 0,0);
    keybd_event('C', MapVirtualKey('C',0), KEYEVENTF_EXTENDEDKEY | 0,0);
    keybd_event('C', MapVirtualKey('C',0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
    keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
    Sleep(200);
    text = clipboard->text(QClipboard::Clipboard);
#elif defined(APP_WM_COCOCA)
    QFile file(APP_OSX_TEXTFILE);
    if(!file.open(QFile::ReadOnly | QFile::Text))
        qWarning() << "Cannot open temp file at " << APP_OSX_TEXTFILE;

    text = file.readAll();
    file.close();
    if(!file.remove())
        qWarning() << "Cannot remove temp file at " << APP_OSX_TEXTFILE;
#endif
    return text;
}
