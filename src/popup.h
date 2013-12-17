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
    QPoint m_cursor_pos;
    QClipboard *m_clipboard;
    bool  m_cursor_pos_locked;
    QTimer m_hide_timer;
};

#endif // POPUP_H
