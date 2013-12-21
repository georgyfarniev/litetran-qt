#include "pronounce.h"
#include "request.h"
#include <QMediaPlayer>
#include <QTemporaryFile>

Pronounce::Pronounce(QObject *parent) :
    QObject(parent),
    player(new QMediaPlayer(this)),
    file(new QTemporaryFile(this))
{
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(removeTemporaryFile()));
}

void Pronounce::say(const QString &text, const QString &lang)
{
    if(text.isEmpty() || lang.isEmpty() || player->state() == QMediaPlayer::PlayingState || text.size() > 100)
        return;

    const QUrl url  = QString(TTS_URL) + QString("?tl=%1&q=%2").arg(lang, text);
    const QByteArray filedata = Request::GET(url);
    file->open();
    file->setAutoRemove(false);
    file->write(filedata);
    file->close();

    player->setMedia(QUrl::fromLocalFile(file->fileName()));
    player->play();
}

void Pronounce::removeTemporaryFile()
{
    if (player->state() == QMediaPlayer::StoppedState)
        file->remove();
}
