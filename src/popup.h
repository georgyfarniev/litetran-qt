#pragma once

#include <QWidget>
#include "translate.h"

namespace Ui {
class Popup;
}

class QPaintEvent;

class Popup : public QWidget
{
	Q_OBJECT
public:
	explicit Popup(QWidget *parent = 0);
	~Popup();

	void prepareDisplayPopup();
    void display(const Language &sl, const Language &tl, const QString &text);
	void paintEvent(QPaintEvent *e); 
signals:
    void requestShowWindow();
private:
	QPoint mCursorPos;
	Ui::Popup *ui;
};
