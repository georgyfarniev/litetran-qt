#include "texttoolbar.h"
#include "defines.h"
#include <QAction>
#include <QEvent>

TextToolbar::TextToolbar(QWidget *parent) :
    QToolBar(parent),
    action_clear(new QAction(this)),
    action_copy(new QAction(this)),
    action_pronounce(new QAction(this))
{
    action_clear->setIcon(APP_ICON("clear"));
    action_copy->setIcon(APP_ICON("copy"));
    action_pronounce->setIcon(APP_ICON("play"));

    addAction(action_clear);
    addAction(action_copy);
    addAction(action_pronounce);

    connect(action_clear, SIGNAL(triggered()), SIGNAL(requestClear()));
    connect(action_copy, SIGNAL(triggered()), SIGNAL(requestCopy()));
    connect(action_pronounce, SIGNAL(triggered()), SIGNAL(requestPronounce()));

    setIconSize(QSize(16, 16));
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void TextToolbar::changeEvent(QEvent *e) {
    QToolBar::changeEvent(e);
    if(e->type() == QEvent::LanguageChange) {
        action_clear->setText(tr("Clear"));
        action_copy->setText(tr("Copy"));
        action_pronounce->setText(tr("Pronounce"));
    }
}
