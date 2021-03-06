#include "settings.h"
#include "ui_settings.h"
#include <QSettings>

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
    ui->PopupKeySequence->setKeySequence(s.value("PopupKeySequence").toString());
    ui->ShowKeySequence->setKeySequence(s.value("ShowKeySequence").toString());
	s.endGroup();

    connect(ui->ToggleButton, &QPushButton::clicked, [=](){
       const bool enabled = mModel->languages().enabledLanguages().isEmpty();

       for (int i = 0; i < mModel->rowCount(); ++i)
           mModel->setData(mModel->index(i, (int)LanguageComboboxModel::Columns::Name), enabled, Qt::CheckStateRole);

       mModel->reload();
    });
}

Settings::~Settings()
{
	QSettings s;
	s.beginGroup("General");
	s.setValue("ShowInTray", ui->ShowTrayIconCheckbox->isChecked());
	s.setValue("AutoTranslate", ui->AutoTranslateCheckbox->isChecked());
    s.setValue("ShowDictionary", ui->DictionaryCheckbox->isChecked());
	s.setValue("PopupShortcutEnabled", ui->PopupHotkeyCheckbox->isChecked());
	s.setValue("ApplicationShortcutEnabled", ui->ShowHotkeyCheckbox->isChecked());
    s.setValue("PopupKeySequence", ui->PopupKeySequence->keySequence().toString());
    s.setValue("ShowKeySequence", ui->ShowKeySequence->keySequence().toString());
	s.endGroup();
}

void Settings::setModel(LanguageComboboxModel *model)
{
    mModel = model;
    ui->LanguagesListView->setModel(mModel);
    ui->LanguagesListView->setModelColumn(0);
}

bool Settings::getTrayIconEnabled() {return ui->ShowTrayIconCheckbox->isChecked(); }
bool Settings::getAutoTranslateEnabled() {return ui->AutoTranslateCheckbox->isChecked();}
bool Settings::getDictionaryEnabled() {return ui->DictionaryCheckbox->isChecked();}
bool Settings::getPopupShortcutEnabled() {return ui->PopupHotkeyCheckbox->isChecked();}
bool Settings::getAppShortcutEnabled() {return ui->ShowHotkeyCheckbox->isChecked();}
QKeySequence Settings::getPopupShortcut() {return ui->PopupKeySequence->keySequence();}
QKeySequence Settings::getAppShortcut() {return ui->ShowKeySequence->keySequence();}

void Settings::closeEvent(QCloseEvent *)
{
	emit accepted();
}


