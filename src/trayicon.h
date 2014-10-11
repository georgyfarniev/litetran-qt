#pragma once

#include <QSystemTrayIcon>

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = 0);
signals:
    void clicked();
private slots:
    void onActivate(QSystemTrayIcon::ActivationReason reason);
};
