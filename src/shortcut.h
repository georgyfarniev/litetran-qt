#pragma once

#include <QObject>
#include <QKeySequence>

class QxtGlobalShortcut;

class Shortcut : public QObject
{
    Q_OBJECT
public:
    Shortcut(QObject *parent = nullptr);

    QKeySequence shortcut() const;
    void setShortcut(const QKeySequence &shortcut);

    void setEnabled(bool enabled);
signals:
    void activated();
private:
    QxtGlobalShortcut *mQxtShortcut;
    QKeySequence mShortcut;
};

