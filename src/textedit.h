#pragma once

#include <QTextEdit>
#include <QList>

class QPaintEvent;
class QAction;
class QToolButton;

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = 0);
    void addAction(QAction *action);
public slots:
    void copy();
private:
    void paintEvent(QPaintEvent *e);
    QList<QToolButton *>mBtnList;
};
