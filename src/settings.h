#pragma once

#include <QDialog>
#include <QKeySequence>
#include "models.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
	Q_OBJECT
public:
	explicit Settings(QWidget *parent = 0);
	~Settings();

	void setModel(LanguageComboboxModel *model);
	bool getTrayIconEnabled();
	bool getAutoTranslateEnabled();
	bool getDictionaryEnabled();
	bool getPopupShortcutEnabled();
	bool getAppShortcutEnabled();
	QKeySequence getPopupShortcut();
	QKeySequence getAppShortcut();
private:
	void closeEvent(QCloseEvent *) override;
    LanguageComboboxModel *mModel;
	Ui::Settings *ui;
};
