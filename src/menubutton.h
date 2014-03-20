#pragma once

#include <QToolButton>

class QPaintEvent;
class QMouseEvent;

class MenuButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MenuButton(QWidget *parent = 0);

    void setMenu(QMenu *m);
private:
    bool eventFilter(QObject *o, QEvent *e);
    void paintEvent(QPaintEvent *e);
};
