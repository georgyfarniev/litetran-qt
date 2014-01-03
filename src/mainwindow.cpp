#include "mainwindow.h"
#include "texttoolbar.h"
#include "settings.h"
#include "trayicon.h"
#include "translate.h"
#include "popup.h"
#include "pronounce.h"
#include "languagedb.h"
#include "clipboard.h"
#include "defines.h"
#include "ui_mainwindow.h"
#include "qxtglobalshortcut.h"
#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    action_settings(new QAction(this)),
    action_about(new QAction(this)),
    action_exit(new QAction(this)),
    menu_button(new QToolButton(this)),
    menu_root(new QMenu( this)),
    settings(new QSettings(this)),
    ui_translator(NULL),
    translate_shortcut(new QxtGlobalShortcut(this)),
    clipboard(new Clipboard(this)),
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

    setWindowTitle(APP_NAME);
    setWindowIcon(QIcon(":/icons/ui/litetran.png"));

    action_settings->setIcon(QIcon(":/icons/ui/settings.png"));
    action_about->setIcon(QIcon(":/icons/ui/about.png"));
    action_exit->setIcon(QIcon(":/icons/ui/exit.png"));

    menu_button->setMenu(menu_root);
    menu_button->setPopupMode(QToolButton::InstantPopup);
    menu_button->setIcon(QIcon(":/icons/ui/settings.png"));
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
    connect(ui->sourceLanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(ui->resultLanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(translate_shortcut, SIGNAL(activated()), this, SLOT(translate()));

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

    ui->sourceLanguageComboBox->setCurrentText(settings->value("SourceLanguage", DEFAULT_SOURCE_LANGUAGE).toString());
    ui->resultLanguageComboBox->setCurrentText(settings->value("ResultLanguage", DEFAULT_RESULT_LANGUAGE).toString());
    restoreGeometry(settings->value("Geometry").toByteArray());

    updateSettings();
}

MainWindow::~MainWindow()
{
    settings->setValue("SourceLanguage", ui->sourceLanguageComboBox->currentText());
    settings->setValue("ResultLanguage", ui->resultLanguageComboBox->currentText());
    settings->setValue("Geometry", saveGeometry());
    delete ui;
}

void MainWindow::about()
{
    QMessageBox::about(this, about_title, about_text);
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
    popup->freezeCursorPosition();
    QString text;
    if(!applicationInFocus())
        text = clipboard->selectedText();
    else
        text = sourceText();

    ui->sourceTextEdit->setPlainText(text);
    const QString result = translate_engine->translate(text, sourceLanguage(), resultLanguage());
    ui->resultTextBrowser->setHtml(result);

    if(!applicationInFocus())
        popup->show(result);
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

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    if(e->type() ==  QEvent::LanguageChange) {
        ui->retranslateUi(this);
        action_settings->setText(tr("Configure"));
        action_about->setText(tr("About"));
        action_exit->setText(tr("Exit"));
        menu_button->setToolTip(QString(APP_NAME) + QString(" ") + tr("menu"));

        about_title = tr("About LiteTran");
        about_text = tr("LiteTran is a lightweight text translation program."
                        "Just select some text, press hotkey and get translation in small popup!"
                        "\n\nLiteTran is licensed under GPL3\n"
                        "Homepage: https://github.com/flareguner/litetran");
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
    translate_engine->setDictionaryEnabled(settings_dialog->dictionaryEnabled());

    const QString locale = settings_dialog->language();


//     retranslate only if language changed
    if(locale != last_locale) {
        if(ui_translator != NULL) {
            qApp->removeTranslator(ui_translator);
            delete ui_translator;
        }

        ui_translator = new QTranslator();

        if(!ui_translator->load(locale + ".qm", APP_I18N_DIR)) {
            qWarning() << "Cannot load translation for language " << locale;
            return;
        }

        if(!qApp->installTranslator(ui_translator)) {
            qWarning() << "Cannot install translator for language " << locale;
            return;
        }

        last_locale = locale;
    }
}

void MainWindow::pronounceSourceText()
{
    pronounce_engine->say(sourceText(), sourceLanguage());
}

void MainWindow::pronounceResultText()
{
    pronounce_engine->say(resultText(), resultLanguage());
}

void MainWindow::languageChanged()
{
    QString tooltip = APP_NAME;
    tooltip += "\n\n" + ui->sourceLanguageComboBox->currentText();
    tooltip += " - " + ui->resultLanguageComboBox->currentText();

    tray_icon->setToolTip(tooltip);
}
