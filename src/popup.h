#ifndef POPUP_H
#define POPUP_H

#include <QWidget>

#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QCursor>

namespace Ui {
class Popup;
}

class Popup : public QWidget
{
	Q_OBJECT
public:
	explicit Popup(QWidget *parent = 0);
	~Popup();

	void prepareDisplayPopup();
	void display(const QString &sl, const QString &tl, const QString &sc, const QString &tc, const QString &text);
	void disappear();
	void paintEvent(QPaintEvent *e); 
signals:
    void requestShowWindow();
private:
	QPoint mCursorPos;
	Ui::Popup *ui;
};

#endif // POPUP_H
