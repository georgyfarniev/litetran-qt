#include "popup.h"
#include <QToolTip>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolButton>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>

Popup::Popup(QWidget *parent) :
    QFrame(parent),
    cursor_locked(false),
    textLabel(new QTextBrowser(this)),
    fm(textLabel->font()),
    button_copy(new QToolButton(this)),
    button_pronounce(new QToolButton(this))
{
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(hide()));


    QLabel *l2 = new QLabel("LiteTran (tm)", this);
    QHBoxLayout *top_layout = new QHBoxLayout;
    top_layout->addWidget(button_copy);
    top_layout->addWidget(button_pronounce);
    top_layout->addStretch();
    top_layout->addWidget(l2);

    QVBoxLayout *l = new QVBoxLayout;
    l->addLayout(top_layout);
    l->addWidget(textLabel);


    button_copy->setFixedSize(16,16);
    button_pronounce->setFixedSize(16,16);

    this->setLayout(l);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::Popup);
    l->setContentsMargins(0,0,0,0);
    top_layout->setContentsMargins(4,4,2,2);
//    textLabel->setContentsMargins(0,0,0,0);
    button_copy->setContentsMargins(2,2,2,2);
    button_pronounce->setContentsMargins(2,2,2,2);

//    setMinimumWidth(250);
//    setMinimumHeight(250);

    setMaximumWidth(350);
    setMaximumHeight(400);

//    setStyleSheet("background: white;");
//    setStyleSheet("background: white; border: 1px solid black 25px");
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);

    textLabel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    textLabel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textLabel->verticalScrollBar()->setStyleSheet("QScrollBar {width:4px; background: white;}");

}

void Popup::displayText(const QString &text)
{
    //calculate longest string
    textLabel->setText(text);
//    uint sz = (fm.height() * text.split("<br>").size()) + 20;
    uint sz = (fm.height() * text.count("<br>")) + 30;
    qDebug() << "HEIGHT: " << sz;
    this->resize(fm.width(text) + 20, sz);//fm.width(text));

    move(cursor_pos);

    show();
    timer.start(POPUP_MIN_TIMEOUT + (text.split(" ").size() * 1000));
}

void Popup::freezeCursorPosition()
{
    cursor_pos = QCursor::pos();
}
