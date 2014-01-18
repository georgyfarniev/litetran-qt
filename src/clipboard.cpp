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
    clipboard = qApp->clipboard();
}


QString Clipboard::selectedText() const {
    QString text;

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBS)
    text = clipboard->text(QClipboard::Selection);
#elif defined(Q_OS_WIN)
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

    // Release the "Ctrl" keysd
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    Sleep(200);

    text = clipboard->text(QClipboard::Clipboard);

#elif defined(Q_OS_OSX)
// TODO: write osx implementation
#endif

    return text;
}
