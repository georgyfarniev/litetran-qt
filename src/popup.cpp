#include "popup.h"
#include "defines.h"
#include "pronounce.h"
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QPaintEvent>
#include <QDebug>
#include <QAction>
#include <QToolBar>
#include <QToolTip>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QToolButton>
#include <QCursor>
#include <QScrollBar>
#include <QClipboard>
#include <QApplication>

#define TOOLTIP_ROUNDNESS 6

PopupToolBar::PopupToolBar(QWidget *parent) :
    QWidget(parent),
    main_layout(new QHBoxLayout)
{
//    main_layout->setContentsMargins(0, 0, 20, 0);
    main_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout);
    main_layout->addStretch();
}

void PopupToolBar::addAction(QAction *action)
{
    QToolButton *button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(action);
    main_layout->addWidget(button);
}


Popup::Popup(Pronounce *pronounce, QWidget *parent) :
    QWidget(parent),
    pronounce_engine(pronounce),
    text_browser(new QTextBrowser(this)),
    bottom_toolbar(new PopupToolBar(this)),
    action_copy(new QAction(APP_ICON("copy"), "Copy", this)),
    action_speak(new QAction(APP_ICON("play"), "Speak", this)),
    action_open(new QAction(APP_ICON("litetran"), "Open LiteTran", this)),
    action_close(new QAction(APP_ICON("exit"), "About", this))
{
    setWindowFlags(Qt::Popup);
    bottom_toolbar->setContentsMargins(55, 0, 0, 0);
    bottom_toolbar->addAction(action_copy);
    bottom_toolbar->addAction(action_speak);
    bottom_toolbar->addAction(action_open);
    bottom_toolbar->addAction(action_close);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(6, 6, 6, 6);
    main_layout->addWidget(text_browser);
    main_layout->addWidget(bottom_toolbar);

    setLayout(main_layout);

    connect(action_copy, &QAction::triggered, this, &Popup::copy);
    connect(action_speak, &QAction::triggered, this, &Popup::pronounce);
    connect(action_open, &QAction::triggered, this, &Popup::showMainWindow);
    connect(action_close, &QAction::triggered, this, &Popup::disappear);

    this->setMouseTracking(true);
}

void Popup::show(const QString &tl, const QString &text)
{
    lang = tl;
    text_browser->setHtml(text);
    move(QCursor::pos() + QPoint(16, 16));
    QWidget::show();
}

void Popup::copy()
{
    qApp->clipboard()->setText(translatedWord());
}

void Popup::pronounce()
{
    pronounce_engine->say(translatedWord(), lang);
}

void Popup::showMainWindow()
{
    hide();
    parentWidget()->setWindowState(Qt::WindowActive);
    parentWidget()->show();
}

void Popup::disappear()
{
    hide();
}

QString Popup::translatedWord() const
{
    return  text_browser->toPlainText().split("\n").first();
}

void Popup::paintEvent(QPaintEvent *e)
{
    redraw();
    e->accept();
}

void Popup::mouseMoveEvent(QMouseEvent *e)
{
    redraw();
    text_browser->setVerticalScrollBarPolicy((this->underMouse() || text_browser->underMouse()) ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    e->accept();
}

void Popup::redraw()
{
    QPixmap pixmap(size());
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.fillRect(pixmap.rect(), Qt::white);
    painter.setBrush(Qt::black);

    QRect rect = pixmap.rect();
    rect.setHeight(rect.height() - bottom_toolbar->height() - TOOLTIP_ROUNDNESS);

    painter.drawRoundedRect(rect, TOOLTIP_ROUNDNESS, TOOLTIP_ROUNDNESS);

    if (this->underMouse() || text_browser->underMouse()) {
        QRect rect2(rect.topLeft(), rect.bottomRight());
        rect2.setHeight(rect2.height() + bottom_toolbar->height() + TOOLTIP_ROUNDNESS);
        painter.drawRoundedRect(rect2, TOOLTIP_ROUNDNESS, TOOLTIP_ROUNDNESS);
    }
    setMask(pixmap.createMaskFromColor(Qt::white));
}
