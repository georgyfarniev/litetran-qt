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

AutoStart::AutoStart(QObject *parent) :
    QObject(parent),
    settings(NULL)
{
}

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
