#ifndef PRONOUNCE_H
#define PRONOUNCE_H

#include <QObject>

#define TTS_URL "http://translate.google.com/translate_tts"

class QMediaPlayer;
class QTemporaryFile;

class Pronounce : public QObject
{
    Q_OBJECT
public:
    explicit Pronounce(QObject *parent = 0);

    // 100 chars max
    void say(const QString &text, const QString &lang);
private:
    QMediaPlayer *player;
    QTemporaryFile *file;
private slots:
    void removeTemporaryFile();

};

#endif // PRONOUNCE_H
