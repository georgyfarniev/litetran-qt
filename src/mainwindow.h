#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include "models.h"
#include "translate.h"

namespace Ui {
class MainWindow;
}

class TrayIcon;
class TranslateEngine;
class Settings;
class Popup;
class LanguageComboboxModel;
class TranslateEngine;
class QCloseEvent;
class QxtGlobalShortcut;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void createActionsConnections();
	void createTimerConnections();
	void createAsyncConnections();

	void createTrayMenu();
	void readSettings();
	void saveSettings();

	Language mapIndexToLanguage(const int idx);
	Language sourceLanguage();
	Language resultLanguage();
private slots:
    void on_pushButton_clicked();

private:

	Ui::MainWindow *ui;
	QTimer mTranslateTimer;
	QxtGlobalShortcut *mTranslateShortcut;
	QxtGlobalShortcut *mAppearShortcut;

	LanguageVector mLanguages;
	LanguageComboboxModel *mComboboxModel;
	LanguageFilter *mFilter;

	Popup *mPopup;
	TrayIcon *mTrayIcon;
	Settings *mSettings;
	TranslateEngine mEngine;
};

