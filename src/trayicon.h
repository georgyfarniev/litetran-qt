#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>

class QMenu;
class QAction;

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = 0);

    void addAction(QAction *act);
    void addSeparator();
signals:
    void clicked();
    void doubleClicked();
private slots:
    void onActivate(QSystemTrayIcon::ActivationReason reason);
private:
    QMenu *mMenu;
};
#endif // TRAYICON_H
