#include "autostart.h"
#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QApplication>

AutoStart::AutoStart(QObject *parent) :
    QObject(parent),
    settings(NULL)
{
}

void AutoStart::setAutoStart(bool enabled)
{
    Q_UNUSED(enabled)
    //TODO: write OSX implementation
}

bool AutoStart::autoStart()
{
    //TODO: write OSX implementation
    return false;
}
