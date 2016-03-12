#include "settings.h"
#include "ui_settings.h"
#include <QSettings>

namespace constants
{
	const QString translate_key = "trnsl.1.1.20160222T212917Z.dac5812c38fde523.efb3b5e5d4634845e1a6106e891343e83d1423d2";

}

Settings::Settings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Settings)
{
	ui->setupUi(this);

	QSettings s;

	s.beginGroup("General");
	ui->ShowTrayIconCheckbox->setChecked(s.value("ShowInTray", true).toBool());
	ui->AutoTranslateCheckbox->setChecked(s.value("AutoTranslate", true).toBool());
	ui->DictionaryCheckbox->setChecked(s.value("ShowDictionary", true).toBool());
	ui->PopupHotkeyCheckbox->setChecked(s.value("PopupShortcutEnabled", true).toBool());
	ui->ShowHotkeyCheckbox->setChecked(s.value("ApplicationShortcutEnabled", true).toBool());
	s.endGroup();

	connect(this, &QDialog::accepted, [=](){
		QSettings s;
		s.beginGroup("General");
		s.setValue("ShowInTray", ui->ShowTrayIconCheckbox->isChecked());
		s.setValue("AutoTranslate", ui->AutoTranslateCheckbox->isChecked());
		s.setValue("ShowDictionary", ui->DictionaryCheckbox->isChecked());
		s.setValue("PopupShortcutEnabled", ui->PopupHotkeyCheckbox->isChecked());
		s.setValue("ApplicationShortcutEnabled", ui->ShowHotkeyCheckbox->isChecked());
		s.endGroup();
	});
}

Settings::~Settings()
{}

void Settings::setModel(LanguageComboboxModel *model)
{
	ui->LanguagesTreeView->setModel(model);
}

bool Settings::getTrayIconEnabled() {return ui->ShowTrayIconCheckbox->isChecked(); }
bool Settings::getAutoTranslateEnabled() {return ui->AutoTranslateCheckbox->isChecked();}
bool Settings::getDictionaryEnabled() {return ui->DictionaryCheckbox->isChecked();}
bool Settings::getPopupShortcutEnabled() {return ui->PopupHotkeyCheckbox->isChecked();}
bool Settings::getAppShortcutEnabled() {return ui->ShowHotkeyCheckbox->isChecked();}
QKeySequence Settings::getPopupShortcut() {return ui->PopupKeySequence->keySequence();}
QKeySequence Settings::getAppShortcut() {return ui->ShowKeySequence->keySequence();}
QString Settings::getUILanguage() const {return ui->LanguageCombobox->currentText(); }

QString Settings::getTranslateKey() const
{
	if(ui->UseCustomKeysCheckbox->isChecked())
		return ui->YandexTranslateKeyLineEdit->text();
	else
		return constants::translate_key;
}

QString Settings::getDictionaryKey() const
{
	return QString();
}

