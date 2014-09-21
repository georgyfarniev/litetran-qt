#include "clipboard.h"
#include "defines.h"
#include <QDebug>
#include <qt_windows.h>

// TODO: move this class to another file
class WinClipboard {
public:
    inline static void setClipboardText(wchar_t *text)
    {
        OpenClipboard(NULL);
        HGLOBAL hMem;
        wchar_t *buf;
        uint size;

        if(text == NULL)
            return;

        size = (wcslen(text) + 1) * sizeof(wchar_t);
        if(size < 2)
            return;

        if (!EmptyClipboard())
            qWarning() << "EmptyClipboard() failed";

        hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, size);
        if(hMem == NULL) {
            qWarning() << "GlobalAlloc() failed";
            return;
        }

        buf = (wchar_t *)GlobalLock(hMem);
        if(buf == NULL) {
            qWarning() << "GlobalLock() failed";
            GlobalFree(hMem);
            return;
        }

        wcscpy(buf, text);

        if(SetClipboardData(CF_UNICODETEXT, buf) == NULL)
            qWarning() << "SetClipboardData() failed";
        GlobalFree(hMem);
        if(!GlobalUnlock(hMem))
            qWarning() << "GlobalUnlock() failed";
        CloseClipboard();
    }

    inline static wchar_t * getClipboardText()
    {
        OpenClipboard(NULL);
        HANDLE hText;
        LPVOID pText;
        wchar_t *text;

        text = NULL;
        hText = GetClipboardData(CF_UNICODETEXT);
        pText = GlobalLock(hText);

        if(hText == NULL || pText == NULL)
            return (text);

        text = wcsdup((const wchar_t *)pText);
        if(*text == -1)
            return (NULL);

        if(!GlobalUnlock(hText))
            qWarning() << "GlobalUnlock() failed";
        return (text);
        CloseClipboard();
    }

    inline static void clearClipboard()
    {
        OpenClipboard(NULL);
        EmptyClipboard();
        CloseClipboard();
    }
};

Clipboard::Clipboard(QObject *parent) :
    QObject(parent)
{
}

QString Clipboard::selectedText() const {
    // Preserve old text
    wchar_t *old_data = WinClipboard::getClipboardText();

    // Prevent leak of clipboard data if Ctrl+C failed
    WinClipboard::clearClipboard();

    Sleep(200);
    keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_EXTENDEDKEY | 0,0);
    keybd_event('C', MapVirtualKey('C',0), KEYEVENTF_EXTENDEDKEY | 0,0);
    keybd_event('C', MapVirtualKey('C',0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
    keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
//    while((GetAsyncKeyState(MapVirtualKey('C',0) != 0)) && (GetAsyncKeyState(VK_CONTROL) != 0));
    Sleep(200);

    wchar_t *new_data = WinClipboard::getClipboardText();
    const QString text = QString::fromWCharArray(new_data);

    // Restore old text
    WinClipboard::setClipboardText(old_data);
    free(new_data);
    free(old_data);

    return text;
}
