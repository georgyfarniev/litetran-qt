#pragma once

#include <QObject>
#include <QQueue>
#include <QBuffer>

class QMediaPlayer;
class QTemporaryFile;
class Translate;

class Pronounce : public QObject
{
    Q_OBJECT
public:
    explicit Pronounce(Translate *ts, QObject *parent = 0);
    void say(const QString &text, QString lang);
private slots:
    void stateChanged();
private:
    void process();
    Translate *translate_engine;
    QMediaPlayer *player;
    QBuffer *mp3_buffer;
    QQueue<QString> queue;
    QString m_lang;
};
