#pragma once

#include <QObject>

class Clipboard : public QObject
{
	Q_OBJECT
public:
	static QString selectedText();
};
