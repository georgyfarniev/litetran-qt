#include "autostart.h"
#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QApplication>

#define DESKTOP_FILE APP_INSTALL_PREFIX"/share/applications/litetran.desktop"
#define REGISTRY_RUN_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

AutoStart::AutoStart(QObject *parent) :
    QObject(parent)
{
#ifdef APP_WM_WINDOWS
  settings = new QSettings(REGISTRY_RUN_PATH, QSettings::NativeFormat, this);
#endif
}

void AutoStart::setAutoStart(bool enabled)
{
#if defined(APP_WM_X11)
    const QString newpath = xdgAutostartFile();
    qDebug() << "NEWPATH: " << newpath;
    if(enabled)
        QFile::copy(DESKTOP_FILE, newpath);
    else
        QFile::remove(newpath);
#elif defined(APP_WM_WINDOWS)
    if(enabled) {
        settings->setValue("LiteTran", QDir::toNativeSeparators(qApp->applicationFilePath()));
    } else {
        settings->remove("LiteTran");
    }
#elif defined(APP_WM_COCOA)
#endif
}

bool AutoStart::autoStart()
{
#if defined(APP_WM_X11)
    return QFile::exists(xdgAutostartFile());
#elif defined(APP_WM_WINDOWS)
    return !settings->value("LiteTran", QString()).toString().isEmpty();
#elif defined(APP_WM_COCOA)
#endif
}

QString AutoStart::xdgAutostartFile() const
{
    QString dirpath;

    dirpath = qgetenv("XDG_CONFIG_DIRS");
    if (dirpath.isEmpty())
        dirpath =  QDir::homePath() + "/.config/autostart/litetran.desktop";
    else
        dirpath = dirpath + "/autostart/litetran.desktop";

    return dirpath;
}
