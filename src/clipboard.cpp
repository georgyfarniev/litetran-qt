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
    HWND hwnd = GetActiveWindow();
    if (IsWindow(hwnd)) {
//        SendMessage(hwnd, WM_KEYDOWN, VK_CONTROL, 0);
//        SendMessage(hwnd, WM_KEYDOWN, 'C', 0);
//        SendMessage(hwnd, WM_KEYUP, 'C', 0);
//        SendMessage(hwnd, WM_KEYUP, VK_CONTROL, 0);
        SendMessage(hwnd, WM_COPY, 0, 0);

        Sleep(200);
        text = clipboard->text(QClipboard::Clipboard);
    }
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
