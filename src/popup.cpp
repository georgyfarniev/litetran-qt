#include "popup.h"
#include "ui_popup.h"
#include <QDesktopWidget>
#include <QClipboard>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

#define CHARACTER_READ_TIME_FACTOR 500
#define DEFAULT_SIZE (QSize(300, 220))
#define SCREEN_CORNER_PADDING 5


Popup::Popup(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Popup)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Popup);

	connect(ui->CloseButton, &QToolButton::clicked, this, &QWidget::close);
    connect(ui->ShowAppButton, &QToolButton::clicked, [=](bool b){
        emit requestShowWindow();
        hide();
    });

    connect(ui->CopyButton, &QToolButton::clicked, [=](){
        qApp->clipboard()->setText(ui->ResultTextBrowser->toPlainText());
        hide();
    });
}

Popup::~Popup()
{
	delete ui;
}

void Popup::prepareDisplayPopup()
{
	mCursorPos = QCursor::pos();
}

void Popup::display(const QString &sl, const QString &tl, const QString &sc, const QString &tc, const QString &text)
{
	ui->ResultTextBrowser->setHtml(text);

	ui->SourceLanguageLabel->setText(sc);
	ui->ResultLanguageLabel->setText(tc);
	ui->SourceLanguageLabel->setToolTip(sl);
	ui->ResultLanguageLabel->setToolTip(tl);

	mCursorPos = QCursor::pos();

	//DO NOT allow popup to move outside of the screen
	QPoint new_pos(mCursorPos + QPoint(16, 16));
	const QRect intersect = QApplication::desktop()->geometry().intersected(QRect(new_pos, DEFAULT_SIZE));
	if (intersect.height() != this->height())
		new_pos = QPoint(new_pos.x(), new_pos.y() - (this->height() - intersect.height()) - SCREEN_CORNER_PADDING);
	if (intersect.width() != this->width())
		new_pos = QPoint(new_pos.x() - (this->width() - intersect.width())  - SCREEN_CORNER_PADDING, new_pos.y());
	move(new_pos);

	QWidget::show();
	activateWindow();
}

void Popup::disappear()
{
	if (!this->underMouse())
		hide();
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

