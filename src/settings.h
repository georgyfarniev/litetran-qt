#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
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


	bool getAutoTranslateEnabled();

private:
	Ui::Settings *ui;
};

#endif // SETTINGS_H
