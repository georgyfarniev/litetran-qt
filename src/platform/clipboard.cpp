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
	const QString backup = qApp->clipboard()->text();

	//! Sending Ctrl+C key
	PostKeyWithModifiers(KEYBOARD_C_KEY, kCGEventFlagMaskCommand);
	QThread::msleep(100);
	const QString text = QApplication::clipboard()->text();

	qApp->clipboard()->setText(backup);
	return text;
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
    QString text() const
	{
        QString data;
        HANDLE data_handle = GetClipboardData(CF_UNICODETEXT);
        if(data_handle != NULL)
        {
            wchar_t *content = reinterpret_cast<wchar_t*>(GlobalLock(data_handle));
            data = QString::fromWCharArray(content);
            GlobalUnlock(data_handle);
        }
        return data;
	}

    void clear()
	{
		EmptyClipboard();
	}

    bool open()
    {
        return OpenClipboard(NULL);
    }

    void close()
    {
        CloseClipboard();
    }
};

QString Clipboard::selectedText()
{
    WinClipboard clipboard;

    if (!clipboard.open())
    {
        qWarning() << "Unable to open clipboard";
        return QString();
    }

    clipboard.clear();
    clipboard.close();

	Sleep(200);
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_EXTENDEDKEY | 0,0);
	keybd_event('C', MapVirtualKey('C',0), KEYEVENTF_EXTENDEDKEY | 0,0);
	keybd_event('C', MapVirtualKey('C',0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,0);
//    while((GetAsyncKeyState(MapVirtualKey('C',0) != 0)) && (GetAsyncKeyState(VK_CONTROL) != 0));
    Sleep(200);


    if (!clipboard.open())
    {
        qWarning() << "Unable to open clipboard";
        return QString();
    }

    const QString text = clipboard.text();
    clipboard.close();

    return text;
}

#endif
