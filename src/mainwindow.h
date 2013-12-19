#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QAction;
class QToolButton;
class QCloseEvent;
class QClipboard;
class QSettings;
class QxtGlobalShortcut;
class TextToolbar;
class Settings;
class TrayIcon;
class Translate;
class Popup;
class LanguageDB;
class Pronounce;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void about();
    void quit();
    void swap();
    void translate();

    void changeVisibility();
    void updateSettings();
    void pronounceSourceText();
    void pronounceResultText();
private:
    void closeEvent(QCloseEvent *);
    bool applicationInFocus();

    QString sourceLanguage() const;
    QString resultLanguage() const;

    QAction *action_settings;
    QAction *action_about;
    QAction *action_exit;

    QToolButton *menu_button;
    QMenu *menu_root;

    QClipboard *clipboard;

    QSettings *settings;

    QxtGlobalShortcut *translate_shortcut;

    TextToolbar *toolbar_source_text;
    TextToolbar *toolbar_result_text;
    Settings *settings_dialog;
    TrayIcon *tray_icon;

    Translate *translate_engine;
    Pronounce *pronounce_engine;
    Popup *popup;

    LanguageDB *langdb;

    Ui::MainWindow *ui;
};
