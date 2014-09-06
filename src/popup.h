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

class PopupToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit PopupToolBar(QWidget *parent = 0);
    void addAction(QAction *action);
    void addWidget(QWidget *widget);
    void addStretch();
private:
    QHBoxLayout *main_layout;
};

class Popup : public QWidget
{
    Q_OBJECT
public:
    explicit Popup(QWidget *parent = 0);
    void display(const QString &sl, const QString &tl, const QString &text);
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
    PopupToolBar *toolbar;
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
