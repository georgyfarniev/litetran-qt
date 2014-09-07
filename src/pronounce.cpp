#include "pronounce.h"
#include "request.h"
#include <QMediaPlayer>
#include <QTemporaryFile>

#define TTS_URL "http://translate.google.com/translate_tts"
#define TTS_MAXCHAR 100

Pronounce::Pronounce(QObject *parent) :
    QObject(parent),
    player(new QMediaPlayer(this)),
    mp3_buffer(new QBuffer(this))
{
    connect(player, &QMediaPlayer::stateChanged, this, &Pronounce::stateChanged);
}

void Pronounce::say(const QString &text, const QString &lang)
{
    if (text.isEmpty() || player->state() == QMediaPlayer::PlayingState)
        return;

    // Split text by spaces to chunks with size <= TTS_MAXCHAR
    const QStringList words = text.simplified().split(" ", QString::SkipEmptyParts);
    QString chunk;
    foreach(QString word, words) {
        if(chunk.size() + word.size() + 1 > TTS_MAXCHAR) {
            queue.enqueue(chunk.simplified());
            chunk.clear();
        }
        chunk += word + " ";
    }

    if (!chunk.isEmpty())
        queue.enqueue(chunk.simplified());

    m_lang = lang;
    process();
}

void Pronounce::process()
{
    const QString text = queue.dequeue();
    qDebug() << "TEXT: " << text;

    const QUrl url = QString("%1?tl=%2&q=%3").arg(TTS_URL, m_lang, QString(text));
    //TODO: maybe fill buffer with all segments?
    mp3_buffer->close();
    mp3_buffer->setData(Request::GET(url));
    mp3_buffer->open(QBuffer::ReadOnly);
    player->setMedia(NULL, mp3_buffer);
    player->play();
}

void Pronounce::stateChanged()
{
    if(player->state() == QMediaPlayer::StoppedState) {
        if (!queue.isEmpty()) {
            process();
        }
    }
}
