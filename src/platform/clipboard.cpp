#include "defines.h"
#include "clipboard.h"
#include <QString>
#include <QThread>

#ifdef APP_WM_COCOA
#include <ApplicationServices/ApplicationServices.h>
#include <QApplication>
#include <QClipboard>

#define KEYBOARD_C_KEY 0x08

static void PostKeyWithModifiers(CGKeyCode key, CGEventFlags modifiers)
{
		CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);

		CGEventRef keyDown = CGEventCreateKeyboardEvent(source, key, TRUE);
		CGEventSetFlags(keyDown, kCGEventFlagMaskCommand);
		CGEventRef keyUp = CGEventCreateKeyboardEvent(source, key, FALSE);

		CGEventPost(kCGAnnotatedSessionEventTap, keyDown);
		CGEventPost(kCGAnnotatedSessionEventTap, keyUp);

		CFRelease(keyUp);
		CFRelease(keyDown);
		CFRelease(source);
}

//! TODO: restore keyboard after retrieving text
QString Clipboard::selectedText()
{
	//! Sending Ctrl+C key
	PostKeyWithModifiers(KEYBOARD_C_KEY, kCGEventFlagMaskCommand);
	QThread::msleep(100);
	return QApplication::clipboard()->text();
}
#endif

#ifdef APP_WM_X11
#include <QClipboard>
#include <QApplication>

QString Clipboard::selectedText()
{
	return QApplication::clipboard()->text(QClipboard::Selection);
}
#endif

#ifdef Q_OS_WIN32
#include <qt_windows.h>
#include <QDebug>
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



QString Clipboard::selectedText()
{
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
    QString text = QString();

    if (new_data)
        text = QString::fromWCharArray(new_data);

	// Restore old text
    if (old_data)
    {
        WinClipboard::setClipboardText(old_data);
        free(old_data);
    }
    if (new_data)
        free(new_data);

	return text;
}

#endif
