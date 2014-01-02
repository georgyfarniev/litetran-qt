#pragma once

#include <QDialog>

#define DEFAULT_LANGUAGE "English"
#define DEFAULT_SHORTCUT "Ctrl+Shift+T"

class QSettings;
class QCheckBox;
class QComboBox;
class QKeySequenceEdit;
class QDialogButtonBox;
class QLabel;

class Settings : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool shortcutEnabled READ shortcutEnabled)
    Q_PROPERTY(bool trayIconEnabled READ trayIconEnabled)
    Q_PROPERTY(bool dictionaryEnabled READ dictionaryEnabled)
    Q_PROPERTY(QKeySequence shortcut READ shortcut)
    Q_PROPERTY(QString language READ language)
public:
    explicit Settings(QWidget *parent = 0);
    int exec();
    bool shortcutEnabled();
    bool trayIconEnabled();
    bool dictionaryEnabled();
    QKeySequence shortcut() const;
    QString language() const;
private slots:
    void accept();
private:
    void read();
    void changeEvent(QEvent *e);
    QString detectSystemLanguage() const;

    QSettings *settings;
    QString default_language;
    QCheckBox *tray_checkbox;
    QCheckBox *shortcut_checkbox;
    QCheckBox *dictionary_checkbox;
    QKeySequenceEdit *shortcut_edit;
    QComboBox *language_combobox;
    QLabel *language_label;
    QDialogButtonBox *button_box;
};
