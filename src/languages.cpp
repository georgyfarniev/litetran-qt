#include "languages.h"
#include "defines.h"
#include <QDebug>
#include <QSettings>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHeaderView>
#include <QEvent>
#include <QMessageBox>

Languages::Languages(QWidget *parent) :
    QDialog(parent),
    settings(new QSettings()),
    lang_widget(new QListWidget(this)),
    set_button(new QPushButton(this)),
    all_langs(LanguageDB().dump())
{
    QVBoxLayout *main_layout = new QVBoxLayout;
    QDialogButtonBox *button_box = new QDialogButtonBox(this);

    main_layout->addWidget(lang_widget);
    main_layout->addWidget(button_box);
    setLayout(main_layout);

    button_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    button_box->addButton(set_button, QDialogButtonBox::ResetRole);

    foreach(Language lang, all_langs) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(lang.first);
        item->setIcon(APP_FLAG(lang.second));
        item->setData(Qt::UserRole, lang.second);
        lang_widget->addItem(item);
    }

    settings->beginGroup("Languages");
    readSettings();

    lang_widget->setIconSize(QSize(16, 16));
    lang_widget->setWrapping(true);

    connect(button_box, &QDialogButtonBox::accepted, this, &Languages::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &Languages::reject);
    connect(set_button, &QPushButton::clicked, this, &Languages::setUnsetAll);

    setFixedSize(450, 300);
}

int Languages::exec()
{
    readSettings();
    return QDialog::exec();
}

void Languages::accept()
{
    if(languages().size() < 2) {
        QMessageBox::warning(this, "Warning", warning_str);
        return;
    }
    saveSettings();
    QDialog::accept();
}

void Languages::saveSettings()
{
    QStringList list;
    foreach(Language lang, languages())
        list << lang.second;
    settings->setValue("EnabledLanguages", list);
    settings->setValue("Geometry", saveGeometry());
}

void Languages::readSettings()
{
    restoreGeometry(settings->value("Geometry").toByteArray());
    QStringList enabled_langs = settings->value("EnabledLanguages").toStringList();
    for (int i = 0; i < lang_widget->count(); ++i) {
        QListWidgetItem *item = lang_widget->item(i);
        if(enabled_langs.contains(item->data(Qt::UserRole).toString()) || enabled_langs.isEmpty())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }
}

void Languages::setUnsetAll()
{
    const bool empty = languages().isEmpty();
    for (int i = 0; i < lang_widget->count(); ++i) {
        QListWidgetItem *item = lang_widget->item(i);
        item->setCheckState(empty ? Qt::Checked : Qt::Unchecked);
    }
}

void Languages::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    if(e->type() ==  QEvent::LanguageChange) {
        set_button->setText(tr("Set\\Unset All"));
        setWindowTitle(tr("Languages"));
        warning_str = tr("At least two languages should be selected to work correctly.");
    }
}

LanguageList Languages::languages() const
{
    LanguageList list;
    for (int i = 0; i < lang_widget->count(); ++i) {
        QListWidgetItem *item = lang_widget->item(i);
        if(item->checkState() == Qt::Checked)
            list.append(Language(item->text(), item->data(Qt::UserRole).toString()));
    }
    return list;
}

