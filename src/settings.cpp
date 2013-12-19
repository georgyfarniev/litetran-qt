#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
#include <QLocale>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QVBoxLayout>
#include <QFormLayout>


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    settings(new QSettings(this)),
    tray_checkbox(new QCheckBox(tr("Show icon in tray"), this)),
    shortcut_checkbox(new QCheckBox(tr("Popup translate by shortcut"), this)),
    shortcut_edit(new QKeySequenceEdit(this)),
    language_combobox(new QComboBox(this)),
    button_box(new QDialogButtonBox(this))
{
    connect(shortcut_checkbox, SIGNAL(toggled(bool)), shortcut_edit, SLOT(setEnabled(bool)));
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));

    QLabel *lang_label = new QLabel(tr("GUI Language"), this);
    QVBoxLayout *main_layout = new QVBoxLayout;
    QFormLayout *elem_layout = new QFormLayout;

    main_layout->addLayout(elem_layout);
    main_layout->addWidget(button_box);

    elem_layout->addRow(lang_label, language_combobox);
    elem_layout->addRow(shortcut_checkbox, shortcut_edit);
    elem_layout->addRow(tray_checkbox);

    setLayout(main_layout);

    button_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    settings->beginGroup("Settings");
    shortcut_checkbox->setChecked(settings->value("ScanShortcutEnabled", true).toBool());
    shortcut_edit->setKeySequence(settings->value("ScanShortcut", DEFAULT_SHORTCUT).toString());
    tray_checkbox->setChecked(settings->value("TrayIconEnabled", true).toBool());
}

Settings::~Settings()
{
}

bool Settings::shortcutEnabled()
{
    return shortcut_checkbox->isChecked();
}

bool Settings::trayIconEnabled()
{
    return tray_checkbox->isChecked();
}

QKeySequence Settings::shortcut() const
{
    return shortcut_edit->keySequence();
}

QString Settings::language() const
{
    return language_combobox->currentText();
}


QString Settings::detectSystemLanguage() const
{
    return QLocale::languageToString(QLocale().language());
}


void Settings::accept()
{
    settings->setValue("ScanShortcutEnabled", shortcut_checkbox->isChecked());
    settings->setValue("ScanShortcut", shortcut_edit->keySequence().toString());
    settings->setValue("Language", language_combobox->currentText());
    settings->setValue("TrayIconEnabled", tray_checkbox->isChecked());

    QDialog::accept();
}

//void Settings::reset()
//{
//    ui->scanCheckBox->setChecked(true);
//    ui->scanKeySequenceEdit->setKeySequence(QKeySequence::fromString(DEFAULT_SHORTCUT));
//    ui->languageComboBox->setCurrentIndex(0);
//    ui->showTrayIconCheckBox->setChecked(true);
//}
