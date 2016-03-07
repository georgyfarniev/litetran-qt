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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void readSettings();
	void saveSettings();

	QString mapIndexToCode(const int idx);
private:
	Ui::MainWindow *ui;
	Popup *mPopup;
	TrayIcon *mTrayIcon;
	Settings *mSettings;
	LanguageVector mLanguages;
	LanguageComboboxModel *mComboboxModel;
	LanguageFilter *mFilter;
	QxtGlobalShortcut *mShortcut;
	TranslateEngine mEngine;
	QTimer mTranslateTimer;
};

