#pragma once

#include <QMainWindow>
#include <QTimer>

#include "translate.h"
#include "3rdparty/qxtshortcut/qxtglobalshortcut.h"


#include "models.h"
namespace Ui {
class MainWindow;
}

class TrayIcon;
class Settings;
class Popup;
class QCloseEvent;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void createTrayMenu();
	void readSettings();
	void saveSettings();

	Language mapIndexToLanguage(const int idx);
	Language sourceLanguage();
	Language resultLanguage();
private:
	void beginAsyncOperation() {}
	void endAsyncOperation() {}
	void asyncOperationTimeout() {}
	void closeEvent(QCloseEvent *event);

	Ui::MainWindow *ui;
	Popup *mPopup;
	TrayIcon *mTrayIcon;
	Settings *mSettings;
	LanguageVector mLanguages;
	LanguageComboboxModel *mComboboxModel;
	LanguageFilter *mFilter;
	QxtGlobalShortcut *mTranslateShortcut;
	QxtGlobalShortcut *mAppearShortcut;
	TranslateEngine mEngine;
	QTimer mTranslateTimer;

	enum class State
	{
		Working, Idle
	};
};

