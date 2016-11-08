#include "shortcut.h"
#include "3rdparty/qxtshortcut/qxtglobalshortcut.h"

Shortcut::Shortcut(QObject *parent)
    :QObject(parent), mQxtShortcut(nullptr)
{
    setEnabled(true);
}

QKeySequence Shortcut::shortcut() const
{
    return mShortcut;
}

void Shortcut::setShortcut(const QKeySequence &shortcut)
{
    mShortcut = shortcut;

    if (mQxtShortcut && !mShortcut.isEmpty())
        mQxtShortcut->setShortcut(mShortcut);
}

void Shortcut::setEnabled(bool enabled)
{
    if (enabled && !mQxtShortcut)
    {
        mQxtShortcut = new QxtGlobalShortcut(this);
        if (!mShortcut.isEmpty())
            mQxtShortcut->setShortcut(mShortcut);
        connect(mQxtShortcut, SIGNAL(activated()), this, SIGNAL(activated()));
    }
    else if(!enabled && mQxtShortcut)
    {
        disconnect(mQxtShortcut, SIGNAL(activated()), this, SIGNAL(activated()));
        delete mQxtShortcut;
        mQxtShortcut = nullptr;
    }
}
