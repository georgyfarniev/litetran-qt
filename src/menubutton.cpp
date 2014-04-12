#include "menubutton.h"
#include "defines.h"
#include <QMenu>
#include <QEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOption>
#include <QThread>

MenuButton::MenuButton(QWidget *parent) :
    QToolButton(parent)
{
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
    setIcon(APP_ICON("menu"));
}

void MenuButton::setMenu(QMenu *m)
{
    QToolButton::setMenu(m);
    m->installEventFilter(this);
}

bool MenuButton::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Show) {
        QMenu *event_menu = static_cast<QMenu *>(o);
        if(event_menu == NULL)
            return false;
        const QPoint point = parentWidget()->mapToGlobal(geometry().bottomRight());
        event_menu->move(point.x() - event_menu->width() + 1, point.y() + 2);
    }
    return QToolButton::eventFilter(o, e);
}

void MenuButton::paintEvent(QPaintEvent *e)
{
    // Code below is taken from cantata media player
    // Hack to work-around Qt5 sometimes leaving toolbutton in 'raised' state.
    QStylePainter painter(this);
    QStyleOptionToolButton opttion;
    initStyleOption(&opttion);
    if (opttion.state & QStyle::State_MouseOver && this != QApplication::widgetAt(QCursor::pos())) {
        opttion.state &=~ QStyle::State_MouseOver;
    }
    painter.drawComplexControl(QStyle::CC_ToolButton, opttion);
}
