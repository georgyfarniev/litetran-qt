#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QFontMetrics>

#define POPUP_MIN_TIMEOUT 5000
#define POPUP_WIDTH 640

class Popup : public QObject
{
    Q_OBJECT
public:
    explicit Popup(QObject *parent = 0);
public slots:
    void show(const QString &text);
    void freezeCursorPosition();
private  slots:
    void hidePopup();
private:
    QString formatText(const QString &text) const;
    QFontMetrics fm;
    QPoint cursor_pos;
    QTimer timer;
    bool cursor_locked;
};
