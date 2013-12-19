#include "mainwindow.h"
#include "texttoolbar.h"
#include "settings.h"
#include "trayicon.h"
#include "translate.h"
#include "popup.h"
#include "pronounce.h"
#include "languagedb.h"
#include "defines.h"
#include "ui_mainwindow.h"
#include "qxtglobalshortcut.h"
#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QClipboard>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    action_settings(new QAction(QIcon(":/icons/ui/settings.png"), tr("Settings"), this)),
    action_about(new QAction(QIcon(":/icons/ui/about.png"), tr("About"), this)),
    action_exit(new QAction(QIcon(":/icons/ui/exit.png"), tr("Exit"), this)),
    menu_button(new QToolButton(this)),
    menu_root(new QMenu( this)),
    clipboard(qApp->clipboard()),
    settings(new QSettings(this)),
    translate_shortcut(new QxtGlobalShortcut(this)),
    toolbar_source_text(new TextToolbar(this)),
    toolbar_result_text(new TextToolbar(this)),
    settings_dialog(new Settings(this)),
    tray_icon(new TrayIcon(this)),
    translate_engine(new Translate(this)),
    pronounce_engine(new Pronounce(this)),
    popup(new Popup(this)),
    langdb(new LanguageDB(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(APP_NAME_FULL);
    setWindowIcon(QIcon(":/icons/ui/litetran.png"));
    tray_icon->setIcon(QIcon(":/icons/ui/litetran.png"));

    menu_button->setMenu(menu_root);
    menu_button->setText(tr("Options"));
    menu_button->setPopupMode(QToolButton::InstantPopup);
    menu_button->setIcon(QIcon::fromTheme("configure"));

    menu_root->addAction(action_settings);
    menu_root->addAction(action_about);
    menu_root->addAction(action_exit);

    ui->topLayout->addWidget(toolbar_source_text);
    ui->topLayout->addStretch();
    ui->topLayout->addWidget(menu_button);

    ui->middlelLayout->insertWidget(0, toolbar_result_text);

    action_exit->setShortcut(QKeySequence("Ctrl+Q"));
    ui->translateButton->setShortcut(QKeySequence("Ctrl+T"));
    ui->swapButton->setShortcut(QKeySequence("Ctrl+Shift+S"));

    connect(action_settings, SIGNAL(triggered()), settings_dialog, SLOT(exec()));
    connect(action_exit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(action_about, SIGNAL(triggered()), this, SLOT(about()));
    connect(tray_icon, SIGNAL(doubleClicked()), this, SLOT(changeVisibility()));
    connect(settings_dialog, SIGNAL(accepted()), this, SLOT(updateSettings()));

    connect(toolbar_source_text, SIGNAL(requestClear()), ui->sourceTextEdit, SLOT(clear()));
    connect(toolbar_result_text, SIGNAL(requestClear()), ui->resultTextBrowser, SLOT(clear()));
    connect(toolbar_source_text, SIGNAL(requestCopy()), ui->sourceTextEdit, SLOT(copy()));
    connect(toolbar_result_text, SIGNAL(requestCopy()), ui->resultTextBrowser, SLOT(copy()));
    connect(toolbar_source_text, SIGNAL(requestPronounce()), this, SLOT(pronounceSourceText()));
    connect(toolbar_result_text, SIGNAL(requestPronounce()), this, SLOT(pronounceResultText()));

    connect(ui->translateButton, SIGNAL(pressed()), this, SLOT(translate()));
    connect(ui->swapButton, SIGNAL(clicked()), this, SLOT(swap()));

    connect(translate_shortcut, SIGNAL(activated()), this, SLOT(translate()));

    connect(ui->sourceTextEdit, SIGNAL(textChanged()), this, SLOT(inputChanged()));
    connect(ui->sourceLanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(inputChanged()));
    connect(ui->resultLanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(inputChanged()));

    tray_icon->addAction(action_exit);
    tray_icon->addAction(action_about);
    tray_icon->addAction(action_settings);

    toolbar_source_text->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    toolbar_result_text->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    LanguageList langs = langdb->dump();
    foreach(Language lang, langs) {
        const QString name = lang.first;
        const QString code = lang.second;
        ui->sourceLanguageComboBox->addItem(QIcon(QString(":/icons/flags/%1.png").arg(code)), name, code);
        ui->resultLanguageComboBox->addItem(QIcon(QString(":/icons/flags/%1.png").arg(code)), name, code);
    }

    settings->beginGroup("MainWindow");

    ui->sourceLanguageComboBox->setCurrentIndex(settings->value("SourceLanguageIndex", 0).toInt());
    ui->resultLanguageComboBox->setCurrentIndex(settings->value("ResultLanguageIndex", 0).toInt());
    restoreGeometry(settings->value("Geometry").toByteArray());

    updateSettings();
}

MainWindow::~MainWindow()
{
    settings->setValue("SourceLanguageIndex", ui->sourceLanguageComboBox->currentIndex());
    settings->setValue("ResultLanguageIndex", ui->resultLanguageComboBox->currentIndex());
    settings->setValue("Geometry", saveGeometry());
    delete ui;
}

void MainWindow::about()
{
    QMessageBox::information(this, "About LiteTran", "LiteTran is a lightweight language translation program.");
}

void MainWindow::quit()
{
    qApp->quit();
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
    const QString sl = sourceLanguage();
    const QString tl = resultLanguage();
    QString text;
    if(!applicationInFocus())
        text = clipboard->text(QClipboard::Selection);
    else
        text = sourceText();

    ui->sourceTextEdit->setPlainText(text);
    const QString result = translate_engine->translate(text, sl, tl);
    ui->resultTextBrowser->setText(result);

    if(!applicationInFocus())
        popup->show(result);
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

bool MainWindow::applicationInFocus()
{
    return (isActiveWindow() || settings_dialog->isActiveWindow());
}

QString MainWindow::sourceLanguage() const
{
    return ui->sourceLanguageComboBox->currentData().toString();
}

QString MainWindow::resultLanguage() const
{
    return ui->resultLanguageComboBox->currentData().toString();
}

QString MainWindow::sourceText() const
{
    return ui->sourceTextEdit->toPlainText();
}

QString MainWindow::resultText() const
{
    return ui->resultTextBrowser->toPlainText();
}

void MainWindow::updateSettings()
{
    tray_icon->setVisible(settings_dialog->trayIconEnabled());
    translate_shortcut->setShortcut(settings_dialog->shortcut());
    translate_shortcut->setEnabled(settings_dialog->shortcutEnabled());
}

void MainWindow::pronounceSourceText()
{
    pronounce_engine->say(sourceText(), sourceLanguage());
}

void MainWindow::pronounceResultText()
{
    pronounce_engine->say(resultText(), resultLanguage());
}

void MainWindow::inputChanged()
{
    ui->translateButton->setEnabled(!sourceText().isEmpty() && sourceLanguage() != resultLanguage());
}
