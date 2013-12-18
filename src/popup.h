#ifndef POPUP_H
#define POPUP_H

#include <QObject>
#include <QtCore/QPoint>
#include <QTimer>

class QClipboard;


class Popup : public QObject
{
    Q_OBJECT
public:
    explicit Popup(QObject *parent = 0);
public slots:
    void show(const QString &text);
    void setLocked(bool b);
private  slots:
    void updateCursorPos();
    void slotHideToolTip();
private:
    QPoint cursor_pos;
    QTimer timer;
    QClipboard *clipboard;
    bool  cursor_locked;
};

#endif // POPUP_H
