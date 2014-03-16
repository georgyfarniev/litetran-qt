#pragma once

#include <QDialog>
#include "languagedb.h"

class QListWidget;
class QSettings;
class QPushButton;

class Languages : public QDialog
{
    Q_OBJECT
public:
    explicit Languages(QWidget *parent = 0);
    LanguageList languages() const;
public slots:
    int exec();
    void accept();
private slots:
    void saveSettings();
    void readSettings();
    void setUnsetAll();
private:
    void changeEvent(QEvent *e);
    QString warning_str;
    QSettings *settings;
    QListWidget *lang_widget;
    QPushButton *set_button;
    LanguageList all_langs;
};
