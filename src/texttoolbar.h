#pragma once

#include <QToolBar>

class QAction;

class TextToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit TextToolbar(QWidget *parent = 0);
signals:
    void requestClear();
    void requestCopy();
    void requestPronounce();
private:
    QAction *action_clear;
    QAction *action_copy;
    QAction *action_pronounce;
};
