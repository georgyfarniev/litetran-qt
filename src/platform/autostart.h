#pragma once

#include "defines.h"
#include <QObject>

class AutoStart : public QObject
{
	Q_OBJECT
public:
	static void setAutoStart(bool enabled);
	static bool autoStart();
};
