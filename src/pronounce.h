#pragma once

#include <QObject>

#define TTS_URL "http://translate.google.com/translate_tts"
#define TTS_MAXCHAR 100

class QMediaPlayer;
class QTemporaryFile;

class Pronounce : public QObject
{
    Q_OBJECT
public:
    explicit Pronounce(QObject *parent = 0);
    void say(const QString &text, const QString &lang);
private:
    QMediaPlayer *player;
    QTemporaryFile *file;
private slots:
    void removeTemporaryFile();
};
