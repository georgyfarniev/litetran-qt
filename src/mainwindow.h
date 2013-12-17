#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QAction;
class QToolButton;
class QCloseEvent;
class QClipboard;
class QTimer;
class QSettings;
class TextToolbar;
class Settings;
class TrayIcon;
class Translate;
class Popup;
class LanguageDB;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void about();
    void swap();
    void translate();
    void changeVisibility();
    void updateSettings();

    void translateSelectedText();
private:
    void closeEvent(QCloseEvent *);

    QAction *action_scan;
    QAction *action_settings;
    QAction *action_about;
    QAction *action_exit;

    QToolButton *menu_button;
    QMenu *menu_root;

    QClipboard *clipboard;
    QTimer *popup_timer;

    QSettings *settings;

    TextToolbar *toolbar_source_text;
    TextToolbar *toolbar_result_text;
    Settings *settings_dialog;
    TrayIcon *tray_icon;

    Translate *translate_engine;
    Popup *popup;

    LanguageDB *langdb;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
