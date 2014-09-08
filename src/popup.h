#pragma once

#include <QWidget>
#include <QTimer>
#include <QRect>

class QEvent;
class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QToolBar;
class QAction;
class QTextBrowser;
class QLabel;
class QHBoxLayout;

class Popup : public QWidget
{
    Q_OBJECT
public:
    explicit Popup(QWidget *parent = 0);
    void display(const QString &sl, const QString &tl, const QString &sc, const QString &tc, const QString &text);
    void recordCursorPosition();
private slots:
    void copy();
    void showMainWindow();
    void disappear();
signals:
    void pronounceRequested();
private:
    QString getLongestString(const QStringList &lst) const;
    QString translatedWord() const;
    void paintEvent(QPaintEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void changeEvent(QEvent *e);
    QToolBar *toolbar;
    QPoint cursor_pos;
    QTimer disappear_timer;
    QRect screen_geometry;
    QTextBrowser *text_browser;
    QAction *action_copy;
    QAction *action_pronounce;
    QAction *action_open;
    QAction *action_close;
    QLabel *label_text;
    QLabel *label_sl;
    QLabel *label_tl;
};
