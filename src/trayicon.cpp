#include "trayicon.h"
#include "defines.h"

TrayIcon::TrayIcon(QObject *parent) :
    QSystemTrayIcon(parent)
{
    setIcon(QIcon(":/icons/ui/litetran.png"));
	connect(this, &TrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason){
		if(reason == QSystemTrayIcon::Trigger)
			emit clicked();
	});
}
