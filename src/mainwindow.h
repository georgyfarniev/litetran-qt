#pragma once

#include <QMainWindow>

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
    void about();
    void quit();
    void swap();
    void translate();
    void reverse();
    void changeVisibility();
    void updateSettings();
    void pronounceSourceText();
    void pronounceResultText();
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

    TextEdit *source_text;
    TextEdit *result_text;
    QComboBox *source_combobox;
    QComboBox *result_combobox;

    QPushButton *translate_button;
    QToolButton *swap_button;

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
    QxtGlobalShortcut *translate_shortcut_global;
    QxtGlobalShortcut *reverse_shortcut_global;

    Clipboard *clipboard;
    TextToolbar *toolbar_source_text;
    TextToolbar *toolbar_result_text;
    Settings *settings_dialog;
    Languages *languages_dialog;
    TrayIcon *tray_icon;
    Translate *translate_engine;
    Pronounce *pronounce_engine;
    Popup *popup;
protected:
    void closeEvent(QCloseEvent *event);
};
