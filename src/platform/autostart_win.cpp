#include "autostart.h"
#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSettings>

#define REGISTRY_RUN_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

AutoStart::AutoStart(QObject *parent) :
    QObject(parent),
    settings(new QSettings(REGISTRY_RUN_PATH, QSettings::NativeFormat, this))
{
}

void AutoStart::setAutoStart(bool enabled)
{
    if(enabled) {
        settings->setValue("LiteTran", QDir::toNativeSeparators(qApp->applicationFilePath()));
    } else {
        settings->remove("LiteTran");
    }
    settings->sync();
}

bool AutoStart::autoStart()
{
    return !settings->value("LiteTran", QString()).toString().isEmpty();
}
