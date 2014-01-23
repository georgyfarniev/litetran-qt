#pragma once

#include <QToolBar>

class QAction;

class TextToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit TextToolbar(QWidget *parent = 0);
signals:
    void requestCopy();
    void requestPronounce();
private:
    void changeEvent(QEvent *e);
    QAction *action_copy;
    QAction *action_pronounce;
};
