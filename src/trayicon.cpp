#include "trayicon.h"
#include "defines.h"

TrayIcon::TrayIcon(QObject *parent) :
    QSystemTrayIcon(parent)
{
    setIcon(APP_ICON("litetran"));
    connect(this, &TrayIcon::activated, this, &TrayIcon::onActivate);
}

void TrayIcon::onActivate(QSystemTrayIcon::ActivationReason reason) {
    if(reason == QSystemTrayIcon::Trigger)
        emit clicked();
}
