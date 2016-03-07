#pragma once

#include <QMainWindow>
#include <QTimer>

#include "translate.h"


#include "models.h"
namespace Ui {
class MainWindow;
}

class TrayIcon;
class Settings;



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
	TrayIcon *mTrayIcon;
	Settings *mSettings;
	LanguageVector mLanguages;
	LanguageComboboxModel *mComboboxModel;
	LanguageFilter *mFilter;
	TranslateEngine mEngine;
	QTimer mTranslateTimer;
};

