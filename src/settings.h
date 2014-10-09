#pragma once

#include <QDialog>

#define DEFAULT_LANGUAGE "English"
#define DEFAULT_TRANSLATE_SHORTCUT "Alt+T"
#define DEFAULT_REVERSE_SHORTCUT "Alt+R"

class QSettings;
class QCheckBox;
class QComboBox;
class QKeySequenceEdit;
class QDialogButtonBox;
class QLabel;
class QGroupBox;
class AutoStart;

class Settings : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool translateShortcutEnabled READ translateShortcutEnabled)
    Q_PROPERTY(bool reverseShortcutEnabled READ reverseShortcutEnabled)
    Q_PROPERTY(QKeySequence translateShortcut READ translateShortcut)
    Q_PROPERTY(QKeySequence reverseShortcut READ reverseShortcut)
    Q_PROPERTY(bool trayIconEnabled READ trayIconEnabled)
    Q_PROPERTY(bool dictionaryEnabled READ dictionaryEnabled)
    Q_PROPERTY(bool autoTranslate READ autoTranslate)
    Q_PROPERTY(bool runAtStartup READ runAtStartup)
    Q_PROPERTY(QString language READ language)
public:
    explicit Settings(QWidget *parent = 0);
    int exec();
    bool translateShortcutEnabled();
    bool reverseShortcutEnabled();
    bool trayIconEnabled();
    bool dictionaryEnabled();
    bool autoTranslate();
    bool runAtStartup();
    QKeySequence translateShortcut() const;
    QKeySequence reverseShortcut() const;
    QString language() const;
private slots:
    void accept();
private:
    void read();
    void changeEvent(QEvent *e);
    QString detectSystemLanguage() const;
    QString msg_key_overlap;
    QString msg_key_overlap_title;
    QString default_language;
    QSettings *settings;
    QCheckBox *tray_checkbox;
    QCheckBox *dictionary_checkbox;
    QCheckBox *autotranslate_checkbox;
    QCheckBox *run_at_startup_checkbox;
    QCheckBox *translate_shortcut_checkbox;
    QCheckBox *reverse_shortcut_checkbox;
    QKeySequenceEdit *translate_shortcut_edit;
    QKeySequenceEdit *reverse_shortcut_edit;
    QComboBox *language_combobox;
    QLabel *label_language;
    QGroupBox *groupbox_app;
    QGroupBox *groupbox_keyboard;
    QDialogButtonBox *button_box;
    AutoStart *autostart_manager;
};
