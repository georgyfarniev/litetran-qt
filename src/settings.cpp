#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
#include <QLocale>
#include <QPushButton>
#include <QDialogButtonBox>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    settings(new QSettings(this))
{
    ui->setupUi(this);

    settings->beginGroup("Settings");
    ui->shortcutEnabledCheckBox->setChecked(settings->value("ScanShortcutEnabled", true).toBool());
    ui->scanKeySequenceEdit->setKeySequence(settings->value("ScanShortcut", DEFAULT_SHORTCUT).toString());
    ui->showTrayIconCheckBox->setChecked(settings->value("TrayIconEnabled", true).toBool());

    const QString lang = settings->value("Language", FALLBACK_LANGUAGE).toString();


    ui->languageComboBox->addItem(lang);

//    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(reset()));
}

Settings::~Settings()
{
    delete ui;
}

bool Settings::shortcutEnabled()
{
    return ui->shortcutEnabledCheckBox->isChecked();
}

bool Settings::trayIconEnabled()
{
    return ui->showTrayIconCheckBox->isChecked();
}

QKeySequence Settings::shortcut() const
{
    return ui->scanKeySequenceEdit->keySequence();
}

QString Settings::language() const
{
    return ui->languageComboBox->currentText();
}


QString Settings::detectSystemLanguage() const
{
    return QLocale::languageToString(QLocale().language());
}


void Settings::accept()
{
    settings->setValue("ScanShortcutEnabled", ui->shortcutEnabledCheckBox->isChecked());
    settings->setValue("ScanShortcut", ui->scanKeySequenceEdit->keySequence().toString());
    settings->setValue("Language", ui->languageComboBox->currentText());
    settings->setValue("TrayIconEnabled", ui->showTrayIconCheckBox->isChecked());

    QDialog::accept();
}

//void Settings::reset()
//{
//    ui->scanCheckBox->setChecked(true);
//    ui->scanKeySequenceEdit->setKeySequence(QKeySequence::fromString(DEFAULT_SHORTCUT));
//    ui->languageComboBox->setCurrentIndex(0);
//    ui->showTrayIconCheckBox->setChecked(true);
//}
