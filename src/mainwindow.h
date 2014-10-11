#pragma once

#include <QMainWindow>
#include <QTimer>

class QTextEdit;
class QTextBrowser;
class QComboBox;
class QPushButton;
class QToolButton;
class QAction;
class QCloseEvent;
class QSettings;
class QTranslator;
class QShortcut;
class QxtGlobalShortcut;
class TextToolbar;
class Settings;
class TrayIcon;
class Translate;
class Popup;
class Pronounce;
class Clipboard;
class TextEdit;
class MenuButton;
class Languages;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(bool collapsed, QWidget *parent = 0);
    ~MainWindow();
private slots:
    void appear();
    void about();
    void quit();
    void swap();
    void translate();
    void timerTranslate();
    void reverse();
    void changeVisibility();
    void updateSettings();
    void copy();
    void pronounce();
    void languageChanged();
    void updateLanguages();
private:
    void translateText(const QString &sl, const QString &tl);
    void changeEvent(QEvent *e);
    bool applicationInFocus();

    QString sourceLanguage() const;
    QString resultLanguage() const;
    QString sourceText() const;
    QString resultText() const;

    QString last_locale;
    QString about_text;
    QString about_title;
    QTimer translate_timer;

    TextEdit *source_text;
    TextEdit *result_text;
    QComboBox *source_combobox;
    QComboBox *result_combobox;

    QPushButton *translate_button;
    QToolButton *swap_button;

    QAction *action_copy;
    QAction *action_pronounce;
    QAction *action_swap;
    QAction *action_settings;
    QAction *action_languages;
    QAction *action_about;
    QAction *action_exit;
    MenuButton *menu_button;
    QMenu *menu_root;
    QSettings *settings;
    QTranslator *ui_translator;
    QShortcut *translate_shortcut;
    QxtGlobalShortcut *shortcut_translate;
    QxtGlobalShortcut *shortcut_reverse;
    QxtGlobalShortcut *shortcut_appear;

    Clipboard *clipboard;
    Settings *settings_dialog;
    Languages *languages_dialog;
    TrayIcon *tray_icon;
    Translate *translate_engine;
    Pronounce *pronounce_engine;
    Popup *popup;
protected:
    void closeEvent(QCloseEvent *event);
};
