#include "mainwindow.h"
#include "texttoolbar.h"
#include "settings.h"
#include "trayicon.h"
#include "translate.h"
#include "popup.h"
#include "languagedb.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QClipboard>
#include <QTimer>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    action_scan(new QAction(tr("Scan selection"), this)),
    action_settings(new QAction(tr("Settings"), this)),
    action_about(new QAction(tr("About"), this)),
    action_exit(new QAction(tr("Exit"), this)),
    menu_button(new QToolButton(this)),
    menu_root(new QMenu( this)),
    clipboard(qApp->clipboard()),
    popup_timer(new QTimer(this)),
    settings(new QSettings(this)),
    toolbar_source_text(new TextToolbar(this)),
    toolbar_result_text(new TextToolbar(this)),
    settings_dialog(new Settings(this)),
    tray_icon(new TrayIcon(this)),
    translate_engine(new Translate(this)),
    popup(new Popup(this)),
    langdb(new LanguageDB(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    menu_button->setMenu(menu_root);
    menu_button->setText(tr("Options"));
    menu_button->setPopupMode(QToolButton::InstantPopup);
    menu_button->setIcon(QIcon::fromTheme("configure"));

    action_scan->setCheckable(true);

    menu_root->addAction(action_scan);
    menu_root->addAction(action_settings);
    menu_root->addAction(action_about);
    menu_root->addAction(action_exit);

    ui->topLayout->addWidget(toolbar_source_text);
    ui->topLayout->addStretch();
    ui->topLayout->addWidget(menu_button);

    ui->middlelLayout->insertWidget(0, toolbar_result_text);

    connect(action_settings, SIGNAL(triggered()), settings_dialog, SLOT(exec()));
    connect(action_exit, SIGNAL(triggered()), this, SLOT(close()));
    connect(action_about, SIGNAL(triggered()), this, SLOT(about()));
    connect(tray_icon, SIGNAL(doubleClicked()), this, SLOT(changeVisibility()));
    connect(settings_dialog, SIGNAL(accepted()), this, SLOT(updateSettings()));

    connect(toolbar_source_text, SIGNAL(requestClear()), ui->sourceTextEdit, SLOT(clear()));
    connect(toolbar_result_text, SIGNAL(requestClear()), ui->resultTextBrowser, SLOT(clear()));
    connect(toolbar_source_text, SIGNAL(requestCopy()), ui->sourceTextEdit, SLOT(copy()));
    connect(toolbar_result_text, SIGNAL(requestCopy()), ui->resultTextBrowser, SLOT(copy()));


    connect(ui->translateButton, SIGNAL(pressed()), this, SLOT(translate()));
    connect(ui->swapButton, SIGNAL(clicked()), this, SLOT(swap()));

    connect(clipboard, SIGNAL(selectionChanged()), popup_timer, SLOT(start()));
    connect(popup_timer, SIGNAL(timeout()), this, SLOT(translateSelectedText()));

    tray_icon->addAction(action_exit);
    tray_icon->addAction(action_settings);
    tray_icon->addAction(action_scan);

    popup_timer->setSingleShot(true);
    popup_timer->setInterval(1000);


    LanguageMap langs = langdb->languages();

    for(LanguageMap::iterator it = langs.begin(); it != langs.end(); it++) {
        const QString name = it.key();
        const QString code = it.value();
        ui->sourceLanguageComboBox->addItem(QIcon(QString(":/icons/flags/%1.png").arg(code)), name);
        ui->resultLanguageComboBox->addItem(QIcon(QString(":/icons/flags/%1.png").arg(code)), name);
    }


    settings->beginGroup("MainWindow");

    ui->sourceLanguageComboBox->setCurrentIndex(settings->value("SourceLanguageIndex", 0).toInt());
    ui->resultLanguageComboBox->setCurrentIndex(settings->value("ResultLanguageIndex", 0).toInt());

    updateSettings();
}

MainWindow::~MainWindow()
{
    settings->setValue("SourceLanguageIndex", ui->sourceLanguageComboBox->currentIndex());
    settings->setValue("ResultLanguageIndex", ui->resultLanguageComboBox->currentIndex());
    delete ui;
}

void MainWindow::about()
{
    QMessageBox::information(this, "About LiteTran", "LiteTran is a lightweight language translation program.");
}

void MainWindow::swap()
{
    const int idx = ui->sourceLanguageComboBox->currentIndex();
    ui->sourceLanguageComboBox->setCurrentIndex(ui->resultLanguageComboBox->currentIndex());
    ui->resultLanguageComboBox->setCurrentIndex(idx);
}

void MainWindow::translate()
{
    popup->setLocked(true);
    const QString sl = langdb->languages()[ui->sourceLanguageComboBox->currentText()];
    const QString tl = langdb->languages()[ui->resultLanguageComboBox->currentText()];
    const QString text = translate_engine->translate(ui->sourceTextEdit->toPlainText(), sl, tl);
    ui->resultTextBrowser->setText(text);

    if(!isActiveWindow() || !settings_dialog->isActiveWindow())
        popup->show(text);
    popup->setLocked(false);
}

void MainWindow::changeVisibility()
{
    setVisible(!isVisible());
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (settings_dialog->trayIconEnabled() && isVisible()) {
        e->ignore();
        hide();
    } else {
        e->accept();
        qApp->quit();
    }
}

void MainWindow::updateSettings()
{
    tray_icon->setVisible(settings_dialog->trayIconEnabled());
}

void MainWindow::translateSelectedText()
{
    if(action_scan->isChecked()) {
        if(isActiveWindow() || settings_dialog->isActiveWindow())
            return;

        ui->sourceTextEdit->setPlainText(clipboard->text(QClipboard::Selection));
        translate();
    }
}
