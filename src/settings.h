#ifndef SETTINGS_H
#define SETTINGS_H

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

	// settings data
	void setModel(LanguageComboboxModel *model);
	bool getTrayIconEnabled();
	bool getAutoTranslateEnabled();
	bool getDictionaryEnabled();

	bool getPopupShortcutEnabled();
	bool getAppShortcutEnabled();

	QKeySequence getPopupShortcut();
	QKeySequence getAppShortcut();

	QString getUILanguage() const;

	QString getTranslateKey() const;
	QString getDictionaryKey() const;
	void closeEvent(QCloseEvent *) override;

private slots:

private:
	Ui::Settings *ui;
};

#endif // SETTINGS_H
