#include "trayicon.h"
#include "defines.h"
#include <QMenu>
#include <QAction>

TrayIcon::TrayIcon(QObject *parent) :
    QSystemTrayIcon(parent),
    menu(new QMenu())
{
    setContextMenu(menu);
    setIcon(APP_ICON("litetran"));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivate(QSystemTrayIcon::ActivationReason)));
}

TrayIcon::~TrayIcon()
{
    delete menu;
}

void TrayIcon::addAction(QAction *act) {
    if(menu->actions().isEmpty())
        menu->addAction(act);
    else
        menu->insertAction(menu->actions().first(), act);
}

void TrayIcon::addSeparator() {
    menu->addSeparator();
}

void TrayIcon::onActivate(QSystemTrayIcon::ActivationReason reason) {
    if(reason == QSystemTrayIcon::DoubleClick)
        emit doubleClicked();
    else if(reason == QSystemTrayIcon::Trigger)
        emit clicked();
}
