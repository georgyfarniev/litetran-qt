#ifndef CLIPBOARD_H
#define CLIPBOARD_H

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

#endif // CLIPBOARD_H
