#include "autostart.h"
#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QApplication>

#define DESKTOP_FILE APP_INSTALL_PREFIX"/share/applications/litetran.desktop"
#define XDG_AUTOSTART_DIR (QDir::homePath() + "/.config/autostart/")
#define XDG_AUTOSTART_FILE (XDG_AUTOSTART_DIR + "/litetran.desktop")

#ifdef APP_WM_COCOA
void AutoStart::setAutoStart(bool enabled)
{
	Q_UNUSED(enabled)
}

bool AutoStart::autoStart()
{
}
#endif

#ifdef APP_WM_X11
void AutoStart::setAutoStart(bool enabled)
{
	QDir autostart_dir(QDir::homePath());

	if (!autostart_dir.exists(".config/autostart") && !autostart_dir.mkpath(".config/autostart")) {
		qWarning() << "Unable to create XDG AutoStart directory: " << XDG_AUTOSTART_DIR;
		return;
	}

	if (enabled) {
		if (!QFile::copy(DESKTOP_FILE, XDG_AUTOSTART_FILE))
			qWarning() << "Unable to add XDG AutoStart record";
	} else {
		if (!QFile::remove(XDG_AUTOSTART_FILE))
			qWarning() << "Unable to add XDG AutoStart record";
	}
}

bool AutoStart::autoStart()
{
	qDebug() << XDG_AUTOSTART_FILE;
	return QFile::exists(XDG_AUTOSTART_FILE);
}
#endif

#ifdef APP_WM_WIN
#define REGISTRY_RUN_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

void AutoStart::setAutoStart(bool enabled)
{
	QSettings settings(REGISTRY_RUN_PATH, QSettings::NativeFormat);
	if(enabled) {
		settings.setValue("LiteTran", QDir::toNativeSeparators(qApp->applicationFilePath()));
	} else {
		settings.remove("LiteTran");
	}
	settings->sync();
}

bool AutoStart::autoStart()
{
	QSettings settings(REGISTRY_RUN_PATH, QSettings::NativeFormat);
	return !settings.value("LiteTran", QString()).toString().isEmpty();
}

#endif
