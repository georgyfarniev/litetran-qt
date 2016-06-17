#include "popup.h"
#include "ui_popup.h"
#include <QDesktopWidget>
#include <QClipboard>
#include <QPainter>
#include <QPaintEvent>

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
    ui->SourceLanguageLabel->setText(sl);
    ui->ResultLanguageLabel->setText(tl);

	mCursorPos = QCursor::pos();

    //Keep popup in screen rect
	QPoint new_pos(mCursorPos + QPoint(16, 16));
    const QRect intersect = QApplication::desktop()->geometry().intersected(QRect(new_pos, size()));
	if (intersect.height() != this->height())
		new_pos = QPoint(new_pos.x(), new_pos.y() - (this->height() - intersect.height()) - SCREEN_CORNER_PADDING);
	if (intersect.width() != this->width())
		new_pos = QPoint(new_pos.x() - (this->width() - intersect.width())  - SCREEN_CORNER_PADDING, new_pos.y());
	move(new_pos);

    show();
	activateWindow();
}

void Popup::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setPen(Qt::gray);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
	painter.end();
	e->accept();
}

