#pragma once

#include <QObject>

class QClipboard;

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = 0);
    QString selectedText() const;
private:
    QClipboard *clipboard;
};
