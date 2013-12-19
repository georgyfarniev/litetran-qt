#include "trayicon.h"

#include <QMenu>
#include <QAction>

TrayIcon::TrayIcon(QObject *parent) :
    QSystemTrayIcon(parent),
    mMenu(new QMenu())
{
    setContextMenu(mMenu);
    setIcon(QIcon::fromTheme("edit-undo"));

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivate(QSystemTrayIcon::ActivationReason)));
}

void TrayIcon::addAction(QAction *act) {
    if(mMenu->actions().isEmpty())
        mMenu->addAction(act);
    else
        mMenu->insertAction(mMenu->actions()[0], act);
}

void TrayIcon::addSeparator() {
    mMenu->addSeparator();
}

void TrayIcon::onActivate(QSystemTrayIcon::ActivationReason reason) {
    if(reason == QSystemTrayIcon::DoubleClick)
        emit doubleClicked();
    else if(reason == QSystemTrayIcon::Trigger)
        emit clicked();
}
