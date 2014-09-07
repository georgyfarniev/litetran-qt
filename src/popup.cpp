#include "popup.h"
#include "defines.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QAction>
#include <QToolBar>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QCursor>
#include <QClipboard>
#include <QApplication>
#include <QLabel>
#include <QDesktopWidget>

#define POPUP_MIN_TIMEOUT 5000
#define DEFAULT_SIZE (QSize(300, 220))
#define UNICODE_ARROW "\u2192"
#define SCREEN_CORNER_PADDING 5

PopupToolBar::PopupToolBar(QWidget *parent) :
    QWidget(parent),
    main_layout(new QHBoxLayout)
{
    main_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout);
}

void PopupToolBar::addAction(QAction *action)
{
    QToolButton *button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(action);
    main_layout->addWidget(button);
}

void PopupToolBar::addWidget(QWidget *widget)
{
    main_layout->addWidget(widget);
}

void PopupToolBar::addStretch()
{
    main_layout->addStretch();
}

Popup::Popup(QWidget *parent) :
    QWidget(parent),
    toolbar(new PopupToolBar(this)),
    screen_geometry(qApp->desktop()->geometry()),
    text_browser(new QTextBrowser(this)),
    action_copy(new QAction(this)),
    action_pronounce(new QAction(this)),
    action_open(new QAction(this)),
    action_close(new QAction(this)),
    label_text(new QLabel(QString::fromUtf8(UNICODE_ARROW), this)),
    label_sl(new QLabel(this)),
    label_tl(new QLabel(this))
{
    action_copy->setIcon(APP_ICON("copy"));
    action_pronounce->setIcon(APP_ICON("play"));
    action_open->setIcon(APP_ICON("litetran"));
    action_close->setIcon(APP_ICON("close"));
    setWindowFlags(Qt::Popup);
    toolbar->setContentsMargins(0, 0, 0, 0);
    toolbar->addAction(action_close);
    toolbar->addAction(action_open);
    toolbar->addAction(action_copy);
    toolbar->addAction(action_pronounce);
    toolbar->addStretch();
    label_sl->setFixedSize(16, 16);
    label_tl->setFixedSize(16, 16);
    toolbar->addWidget(label_sl);
    toolbar->addWidget(label_text);
    toolbar->addWidget(label_tl);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(3, 3, 3, 3);
    main_layout->addWidget(toolbar);
    main_layout->addWidget(text_browser);
    setLayout(main_layout);
    setFixedSize(DEFAULT_SIZE);

    connect(action_copy, &QAction::triggered, this, &Popup::copy);
    connect(action_pronounce, &QAction::triggered, this, &Popup::pronounceRequested);
    connect(action_open, &QAction::triggered, this, &Popup::showMainWindow);
    connect(action_close, &QAction::triggered, this, &QWidget::hide);
    connect(&disappear_timer, &QTimer::timeout, this, &Popup::disappear);


    this->setMouseTracking(true);
    disappear_timer.setSingleShot(true);
}

void Popup::display(const QString &sl, const QString &tl, const QString &sc, const QString &tc, const QString &text)
{
    label_sl->setPixmap(QPixmap(QString(":/icons/flags/%1.png").arg(sc)).scaled(16, 16));
    label_tl->setPixmap(QPixmap(QString(":/icons/flags/%1.png").arg(tc)).scaled(16, 16));

    label_sl->setToolTip(sl);
    label_tl->setToolTip(tl);
    const int words_count = translatedWord().split(" ").size();
    text_browser->setHtml(text);

    QPoint new_pos(cursor_pos + QPoint(16, 16));
    const QRect intersect = screen_geometry.intersected(QRect(new_pos, DEFAULT_SIZE));
    if (intersect.height() != this->height())
        new_pos = QPoint(new_pos.x(), new_pos.y() - (this->height() - intersect.height()) - SCREEN_CORNER_PADDING);

    if (intersect.width() != this->width())
        new_pos = QPoint(new_pos.x() - (this->width() - intersect.width())  - SCREEN_CORNER_PADDING, new_pos.y());
    move(new_pos);

    QWidget::show();
    activateWindow();
    disappear_timer.start(POPUP_MIN_TIMEOUT + (words_count * 1000));
}

void Popup::recordCursorPosition()
{
    cursor_pos = QCursor::pos();
}

void Popup::copy()
{
    const QString text = text_browser->textCursor().hasSelection() ? text_browser->textCursor().selectedText() : translatedWord();
    qApp->clipboard()->setText(text);
}

void Popup::showMainWindow()
{
    hide();
    parentWidget()->show();
    parentWidget()->activateWindow();
}

void Popup::disappear()
{
    if(!this->underMouse())
        hide();
}

QString Popup::getLongestString(const QStringList &lst) const
{
    QString ret;
    foreach(QString current, lst) {
        if (current.size() > ret.size())
            ret = current;
    }
    return ret;
}

QString Popup::translatedWord() const
{
    return  text_browser->toPlainText().split("\n").first();
}

void Popup::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::gray);
    QRect border_rect = this->rect();
    border_rect.adjust(0, 0, -1, -1);
    painter.drawRect(border_rect);
    painter.end();
    e->accept();
}

void Popup::mouseReleaseEvent(QMouseEvent *e)
{
    if (!this->underMouse())
        hide();
    e->accept();
}


void Popup::keyReleaseEvent(QKeyEvent *e)
{
    hide();
    e->accept();
}


void Popup::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    if(e->type() ==  QEvent::LanguageChange) {
        action_copy->setText(tr("Copy"));
        action_pronounce->setText(tr("Pronounce"));
        action_open->setText(tr("Open LiteTran"));
        action_close->setText(tr("Close Popup"));
    }
}
