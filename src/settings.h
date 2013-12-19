#pragma once

#include <QDialog>

#define FALLBACK_LANGUAGE "English"
#define DEFAULT_SHORTCUT "Ctrl+Shift+T"

namespace Ui {
class Settings;
}

class QSettings;


class Settings : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool shortcutEnabled READ shortcutEnabled)
    Q_PROPERTY(bool trayIconEnabled READ trayIconEnabled)
    Q_PROPERTY(QKeySequence shortcut READ shortcut)
    Q_PROPERTY(QString language READ language)
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    bool shortcutEnabled();
    bool trayIconEnabled();
    QKeySequence shortcut() const;
    QString language() const;
private slots:
    void accept();
//    void reset();
private:
    QString detectSystemLanguage() const;


    Ui::Settings *ui;
    QSettings *settings;
};
