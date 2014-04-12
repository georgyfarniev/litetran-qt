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
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDir>
#include <QDebug>
#include <QSpacerItem>
#include <QMessageBox>
#include <QKeySequenceEdit>


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    settings(new QSettings(this)),
    tray_checkbox(new QCheckBox(this)),
    dictionary_checkbox(new QCheckBox(this)),
    translate_shortcut_checkbox(new QCheckBox(this)),
    reverse_shortcut_checkbox(new QCheckBox(this)),
    translate_shortcut_edit(new QKeySequenceEdit(this)),
    reverse_shortcut_edit(new QKeySequenceEdit(this)),
    language_combobox(new QComboBox(this)),
    language_label(new QLabel(this)),
    button_box(new QDialogButtonBox(this))
{
    connect(translate_shortcut_checkbox, SIGNAL(toggled(bool)), translate_shortcut_edit, SLOT(setEnabled(bool)));
    connect(reverse_shortcut_checkbox, SIGNAL(toggled(bool)), reverse_shortcut_edit, SLOT(setEnabled(bool)));
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));

    QFormLayout *elem_layout = new QFormLayout;    
    elem_layout->addRow(language_label, language_combobox);
    elem_layout->addRow(translate_shortcut_checkbox, translate_shortcut_edit);
    elem_layout->addRow(reverse_shortcut_checkbox, reverse_shortcut_edit);
    elem_layout->addRow(tray_checkbox, new QWidget(this));
    elem_layout->addRow(dictionary_checkbox, new QWidget(this));

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addLayout(elem_layout);
    main_layout->addWidget(button_box);
    main_layout->setSizeConstraint(QLayout::SetFixedSize);
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

    read();
}

int Settings::exec() {
    read();
    return QDialog::exec();
}

bool Settings::translateShortcutEnabled()
{
    return translate_shortcut_checkbox->isChecked();
}

bool Settings::reverseShortcutEnabled()
{
    return reverse_shortcut_checkbox->isChecked();
}

bool Settings::trayIconEnabled()
{
    return tray_checkbox->isChecked();
}

bool Settings::dictionaryEnabled()
{
    return dictionary_checkbox->isChecked();
}

QKeySequence Settings::translateShortcut() const
{
    return translate_shortcut_edit->keySequence();
}

QKeySequence Settings::reverseShortcut() const
{
    return reverse_shortcut_edit->keySequence();
}

QString Settings::language() const
{
    return language_combobox->currentText();
}

QString Settings::detectSystemLanguage() const
{
    return QLocale::languageToString(QLocale().language());
}

void Settings::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    if(e->type() == QEvent::LanguageChange) {
            msg_key_overlap = tr("Translate and reverse key sequences shouldn't be overlapped!");
            msg_key_overlap_title = tr("Invalid shortcuts");
            tray_checkbox->setText(tr("Show icon in system tray"));
            translate_shortcut_checkbox->setText(tr("Translate"));
            reverse_shortcut_checkbox->setText(tr("Reverse translate"));
            dictionary_checkbox->setText(tr("Show dictionary results"));
            language_label->setText(tr("Application language"));
            setWindowTitle(tr("Configure"));
    }
}

void Settings::accept()
{
    if(translateShortcut() == reverseShortcut()) {
        QMessageBox::warning(this, msg_key_overlap_title, msg_key_overlap);
        return;
    }
    settings->setValue("TranslateShortcutEnabled", translate_shortcut_checkbox->isChecked());
    settings->setValue("ReverseShortcutEnabled", reverse_shortcut_checkbox->isChecked());
    settings->setValue("TranslateShortcut", translate_shortcut_edit->keySequence().toString());
    settings->setValue("ReverseShortcut", reverse_shortcut_edit->keySequence().toString());


    settings->setValue("Language", language_combobox->currentText());
    settings->setValue("TrayIconEnabled", tray_checkbox->isChecked());
    settings->setValue("ShowDictionary", dictionary_checkbox->isChecked());

    QDialog::accept();
}

void Settings::read()
{
    language_combobox->setCurrentText(settings->value("Language", default_language).toString());


    translate_shortcut_checkbox->setChecked(settings->value("TranslateShortcutEnabled", true).toBool());
    reverse_shortcut_checkbox->setChecked(settings->value("ReverseShortcutEnabled", true).toBool());

    translate_shortcut_edit->setKeySequence(settings->value("TranslateShortcut", DEFAULT_TRANSLATE_SHORTCUT).toString());
    reverse_shortcut_edit->setKeySequence(settings->value("ReverseShortcut", DEFAULT_REVERSE_SHORTCUT).toString());


    tray_checkbox->setChecked(settings->value("TrayIconEnabled", true).toBool());
    dictionary_checkbox->setChecked(settings->value("ShowDictionary", false).toBool());
}
