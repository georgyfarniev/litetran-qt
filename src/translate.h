#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <QString>
#include <QObject>

class Translate : public QObject
{
    Q_OBJECT
public:
    explicit Translate(QObject *parent = 0);

    QString translate(const QString &text, const QString &sl, const QString &tl) const;
//    QString detect(const QString &text);
};

#endif // TRANSLATE_H
