#ifndef PRONOUNCE_H
#define PRONOUNCE_H

#include <QObject>

class Pronounce : public QObject
{
    Q_OBJECT
public:
    explicit Pronounce(QObject *parent = 0);

    void say(const QString &text, const QString &lang);
};

#endif // PRONOUNCE_H
