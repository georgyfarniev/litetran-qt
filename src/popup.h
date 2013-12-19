#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>

class QClipboard;

//TODO: calculate this values
#define POPUP_HIDE_TIMEOUT 5000
#define POPUP_WIDTH 640

class Popup : public QObject
{
    Q_OBJECT
public:
    explicit Popup(QObject *parent = 0);
public slots:
    void show(const QString &text);
    void setLocked(bool b);
private  slots:
    void updatePosition();
    void hidePopup();
private:
    QString formatText(const QString &text) const;

    QPoint cursor_pos;
    QTimer timer;
    QClipboard *clipboard;
    bool  cursor_locked;
};
