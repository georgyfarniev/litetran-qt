#pragma once

#include <QObject>
#include <QQueue>
#include <QBuffer>

class QMediaPlayer;
class QTemporaryFile;

class Pronounce : public QObject
{
    Q_OBJECT
public:
    explicit Pronounce(QObject *parent = 0);
    void say(const QString &text, const QString &lang);
private slots:
    void stateChanged();
private:
    void process();
    QMediaPlayer *player;
    QBuffer *mp3_buffer;
    QQueue<QString> queue;
    QString m_lang;
};
