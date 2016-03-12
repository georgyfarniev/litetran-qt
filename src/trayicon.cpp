#include "trayicon.h"
#include "defines.h"

TrayIcon::TrayIcon(QObject *parent) :
    QSystemTrayIcon(parent)
{
    setIcon(APP_ICON("litetran"));
	connect(this, &TrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason){
		if(reason == QSystemTrayIcon::Trigger)
			emit clicked();
	});
}
