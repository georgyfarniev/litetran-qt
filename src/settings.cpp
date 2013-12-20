#include "settings.h"
#include "defines.h"
#include <QApplication>
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
#include <QDir>
#include <QDebug>


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

    // Initialize languages combobox.
    QDir qmdir(APP_I18N_DIR);
    const QStringList qmfiles = qmdir.entryList(QStringList() << "*.qm");

    foreach(QString qmfile, qmfiles) {
        qmfile = qmfile.split('.').first();
        language_combobox->addItem(qmfile);
    }

    settings->beginGroup("Settings");

    // Try to set language from locale, English is a fallback language.
    default_language = detectSystemLanguage();
    if(!qmfiles.contains(default_language + ".qm"))
        default_language = "English";
}

Settings::~Settings()
{
}

int Settings::exec() {
    /*
     * Re-read settings in each execution in order to avoid user confusion with mismatch
     * of controls state and really enabled parameters.
     */
    language_combobox->setCurrentText(settings->value("Language", default_language).toString());
    shortcut_checkbox->setChecked(settings->value("ScanShortcutEnabled", true).toBool());
    shortcut_edit->setKeySequence(settings->value("ScanShortcut", DEFAULT_SHORTCUT).toString());
    tray_checkbox->setChecked(settings->value("TrayIconEnabled", true).toBool());
    return QDialog::exec();
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
