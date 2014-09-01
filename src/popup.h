#pragma once

#include <QWidget>
#include <QTextBrowser>

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QToolBar;
class QAction;
class QHBoxLayout;
class QTextBrowser;
class Pronounce;


class PopupToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit PopupToolBar(QWidget *parent = 0);
    void addAction(QAction *action);
    void addStretch();
private:
    QHBoxLayout *main_layout;
};

class Popup : public QWidget
{
    Q_OBJECT
public:
    explicit Popup(Pronounce *pronounce, QWidget *parent = 0);
    void show(const QString &tl, const QString &text);
private slots:
    void copy();
    void pronounce();
    void showMainWindow();
    void disappear();
private:
    QString translatedWord() const;
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

    void redraw();
    QString lang;
    Pronounce *pronounce_engine;
    QTextBrowser *text_browser;
    PopupToolBar *bottom_toolbar;
    QAction *action_copy;
    QAction *action_speak;
    QAction *action_open;
    QAction *action_close;
};
