#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QFontMetrics>
#include <QFrame>

#define POPUP_MIN_TIMEOUT 5000
#define POPUP_WIDTH 640

class QTextBrowser;
class QToolButton;

class Popup : public QFrame
{
    Q_OBJECT
public:
    explicit Popup(QWidget *parent = 0);
public slots:
    void displayText(const QString &text);
    void freezeCursorPosition();
private:
    bool cursor_locked;
    QPoint cursor_pos;
    QTimer timer;
    QTextBrowser *textLabel;
    QFontMetrics fm;

    QToolButton *button_copy;
    QToolButton *button_pronounce;
    QToolButton *button_pin;
};
