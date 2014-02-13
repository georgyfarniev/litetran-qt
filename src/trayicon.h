#pragma once

#include <QSystemTrayIcon>

class QMenu;
class QAction;

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = 0);
    ~TrayIcon();
    void addAction(QAction *act);
    void addSeparator();
signals:
    void clicked();
private slots:
    void onActivate(QSystemTrayIcon::ActivationReason reason);
private:
    QMenu *menu;
};
