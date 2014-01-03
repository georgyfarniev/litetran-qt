#include "clipboard.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#ifdef Q_OS_WIN32
#include <qt_windows.h>
#endif

Clipboard::Clipboard(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_LINUX
    clipboard = qApp->clipboard();
#endif
}


QString Clipboard::selectedText() const {
    QString text;


#ifdef Q_OS_LINUX
    text = clipboard->text(QClipboard::Selection);
#endif

#ifdef Q_OS_WIN
    Sleep(200);
    // Send Ctrl+C to current window
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "Ctrl" key
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Press the "V" key
    ip.ki.wVk = 'C';
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "V" key
    ip.ki.wVk = 'C';
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "Ctrl" key
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    Sleep(200);

    // Receive text from clipboard
    if(OpenClipboard(NULL))
    {
        HGLOBAL hglb = GetClipboardData(CF_TEXT);
        LPSTR lpstr = (LPSTR)GlobalLock(hglb);
        text = QString::fromUtf8(lpstr);
        GlobalUnlock(hglb);
        CloseClipboard();
    }
    else
    {
        qDebug() << "CLIPBOARD OPEN ERROR: "  << GetLastError();
    }
#endif

    return text;
}
