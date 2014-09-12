#pragma once

#include <QObject>
class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = 0);
    QString selectedText() const;
};
