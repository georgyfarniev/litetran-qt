#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Settings)
{
	ui->setupUi(this);
}

Settings::~Settings()
{
	delete ui;
}

void Settings::setModel(LanguageComboboxModel *model)
{
	ui->LanguagesTreeView->setModel(model);
}

bool Settings::getAutoTranslateEnabled()
{
	return ui->AutoTranslateCheckbox->isChecked();
}
