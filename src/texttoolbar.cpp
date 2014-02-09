#include "texttoolbar.h"
#include "defines.h"
#include <QAction>
#include <QEvent>

#include <QDebug>
#include <QStyle>
#include <QCommonStyle>
#include <QProxyStyle>

TextToolbar::TextToolbar(QWidget *parent) :
    QToolBar(parent),
    action_copy(new QAction(this)),
    action_pronounce(new QAction(this))
{
    action_copy->setIcon(APP_ICON("copy"));
    action_pronounce->setIcon(APP_ICON("play"));

    addAction(action_copy);
    addAction(action_pronounce);

    connect(action_copy, SIGNAL(triggered()), SIGNAL(requestCopy()));
    connect(action_pronounce, SIGNAL(triggered()), SIGNAL(requestPronounce()));

    setIconSize(QSize(16, 16));
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

#ifdef Q_OS_MAC
    setStyle(new QCommonStyle());
    setStyleSheet("border: none");
#endif
}

void TextToolbar::changeEvent(QEvent *e) {
    QToolBar::changeEvent(e);
    if(e->type() == QEvent::LanguageChange) {
        action_copy->setText(tr("Copy"));
        action_pronounce->setText(tr("Pronounce"));
    }
}
