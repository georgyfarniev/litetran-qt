#pragma once

#include <QObject>
#include "defines.h"

class QSettings;

class AutoStart : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart)
public:
    explicit AutoStart(QObject *parent = 0);
    void setAutoStart(bool enabled);
    bool autoStart();
private:
    QString xdgAutostartFile() const;
    QSettings *settings;
};
