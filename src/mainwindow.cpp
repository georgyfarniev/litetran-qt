#include "mainwindow.h"
#include "texttoolbar.h"
#include "settings.h"
#include "trayicon.h"
#include "translate.h"
#include "popup.h"
#include "pronounce.h"
#include "languagedb.h"
#include "platform/clipboard.h"
#include "defines.h"
#include "textedit.h"
#include "menubutton.h"
#include "languages.h"
#include "qxtglobalshortcut.h"
#include <QCloseEvent>
#include <QApplication>
#include <QTextEdit>
#include <QTextBrowser>
#include <QComboBox>
#include <QPushButton>
#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QCommonStyle>

#ifdef APP_WM_COCOA
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#endif

#define DEFAULT_SOURCE_LANGUAGE "English"
#define DEFAULT_RESULT_LANGUAGE "Russian"

MainWindow::MainWindow(bool collapsed, QWidget *parent) :
    QMainWindow(parent),
    source_text(new TextEdit(this)),
    result_text(new TextEdit(this)),
    source_combobox(new QComboBox(this)),
    result_combobox(new QComboBox(this)),
    translate_button(new QPushButton(this)),
    swap_button(new QToolButton(this)),
    action_swap(new QAction(this)),
    action_settings(new QAction(this)),
    action_languages(new QAction(this)),
    action_about(new QAction(this)),
    action_exit(new QAction(this)),
    menu_button(new MenuButton(this)),
    menu_root(new QMenu( this)),
    settings(new QSettings(this)),
    ui_translator(NULL),
    translate_shortcut(new QShortcut(this)),
    translate_shortcut_global(new QxtGlobalShortcut(this)),
    reverse_shortcut_global(new QxtGlobalShortcut(this)),
    clipboard(new Clipboard(this)),
    toolbar_source_text(new TextToolbar(this)),
    toolbar_result_text(new TextToolbar(this)),
    settings_dialog(new Settings(this)),
    languages_dialog(new Languages(this)),
    tray_icon(new TrayIcon(this)),
    translate_engine(new Translate(this)),
    pronounce_engine(new Pronounce(translate_engine, this)),
    popup(new Popup(this))
{
#ifdef APP_WM_COCOA
    menu_button->setStyle(new QCommonStyle());
    swap_button->setStyle(new QCommonStyle());
//    menu_button->setStyleSheet("border: 1px solid transparent");
    swap_button->setStyleSheet("border: 1px solid transparent");
#endif
    setWindowTitle(APP_NAME);
    setWindowIcon(APP_ICON("litetran"));
    swap_button->setIcon(APP_ICON("swap"));

    translate_button->setIcon(APP_ICON("translate"));
    action_swap->setIcon(APP_ICON("swap"));
    action_settings->setIcon(APP_ICON("settings"));
    action_languages->setIcon(APP_ICON("languages"));
    action_about->setIcon(APP_ICON("about"));
    action_exit->setIcon(APP_ICON("exit"));

    action_exit->setShortcut(QKeySequence("Ctrl+Q"));
    translate_shortcut->setKey(QKeySequence("Ctrl+T"));
    translate_shortcut->setContext(Qt::ApplicationShortcut);
    swap_button->setShortcut(QKeySequence("Ctrl+Shift+S"));

    menu_button->setMenu(menu_root);
    menu_root->addAction(action_settings);
    menu_root->addAction(action_languages);
    menu_root->addSeparator();
    menu_root->addAction(action_about);
    menu_root->addAction(action_exit);

    QHBoxLayout *top_layout = new QHBoxLayout;
    top_layout->addWidget(toolbar_source_text);
    top_layout->addStretch();
    top_layout->addWidget(menu_button);

    QHBoxLayout *middle_layout = new QHBoxLayout;
    middle_layout->addWidget(toolbar_result_text);
    middle_layout->addWidget(source_combobox);
    middle_layout->addWidget(swap_button);
    middle_layout->addWidget(result_combobox);
    middle_layout->addStretch();
    middle_layout->addWidget(translate_button);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addLayout(top_layout);
    main_layout->addWidget(source_text);
    main_layout->addLayout(middle_layout);
    main_layout->addWidget(result_text);

    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(main_layout);

    result_text->setReadOnly(true);

    connect(action_swap, &QAction::triggered, this, &MainWindow::swap);
    connect(action_settings, &QAction::triggered, settings_dialog, &Settings::exec);
    connect(action_languages, &QAction::triggered, languages_dialog, &Languages::exec);
    connect(action_exit, &QAction::triggered, this, &MainWindow::quit);
    connect(action_about, &QAction::triggered, this, &MainWindow::about);
    connect(translate_shortcut, &QShortcut::activated, translate_button, &QPushButton::click);
    connect(settings_dialog, &QDialog::accepted, this, &MainWindow::updateSettings);
    connect(languages_dialog, &QDialog::accepted, this, &MainWindow::updateLanguages);
    connect(toolbar_source_text, &TextToolbar::requestCopy, source_text, &TextEdit::copyAll);
    connect(toolbar_result_text, &TextToolbar::requestCopy, result_text, &TextEdit::copyAll);
    connect(toolbar_source_text, &TextToolbar::requestPronounce, this, &MainWindow::pronounceSourceText);
    connect(toolbar_result_text, &TextToolbar::requestPronounce, this, &MainWindow::pronounceResultText);
    connect(translate_button, &QPushButton::clicked, this, &MainWindow::translate);
    connect(swap_button, &QPushButton::clicked, this, &MainWindow::swap);
    connect(source_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(result_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(translate_shortcut_global, &QxtGlobalShortcut::activated, this, &MainWindow::translate);
    connect(reverse_shortcut_global, &QxtGlobalShortcut::activated, this, &MainWindow::reverse);
    connect(popup, &Popup::pronounceRequested, this, &MainWindow::pronounceResultText);
    connect(&translate_timer, SIGNAL(timeout()), this, SLOT(timerTranslate()));

    translate_timer.setSingleShot(true);
    translate_timer.setInterval(500);

    tray_icon->addAction(action_settings);
    tray_icon->addSeparator();
    tray_icon->addAction(action_swap);
    tray_icon->addSeparator();
    tray_icon->addAction(action_about);
    tray_icon->addAction(action_exit);

    updateLanguages();
    settings->beginGroup("MainWindow");

    source_combobox->setCurrentText(settings->value("SourceLanguage", DEFAULT_SOURCE_LANGUAGE).toString());
    result_combobox->setCurrentText(settings->value("ResultLanguage", DEFAULT_RESULT_LANGUAGE).toString());
    restoreGeometry(settings->value("Geometry").toByteArray());
#ifdef APP_WM_COCOA
    // there is no need in controlling window visibility manually in OS X
    setVisible(true);
#else
    connect(tray_icon, SIGNAL(clicked()), this, SLOT(changeVisibility()));
    setVisible((settings->value("Visible", true).toBool() || !settings_dialog->trayIconEnabled()) && !collapsed);
#endif

    updateSettings();
}

MainWindow::~MainWindow()
{
    settings->setValue("SourceLanguage", source_combobox->currentText());
    settings->setValue("ResultLanguage", result_combobox->currentText());
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("Visible", isVisible());
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
    const int idx = source_combobox->currentIndex();
    source_combobox->setCurrentIndex(result_combobox->currentIndex()  + 1);
    result_combobox->setCurrentIndex(idx - 1);
}

void MainWindow::translateText(const QString &sl, const QString &tl)
{
    popup->recordCursorPosition();

    if(!applicationInFocus())
        source_text->setPlainText(clipboard->selectedText());

    const QString result = translate_engine->translate(sourceText(), sl, tl);
    result_text->setHtml(result);

    if(!applicationInFocus() && !result.isEmpty())
        popup->display(source_combobox->currentText(), result_combobox->currentText(), sl, tl, result);
}

void MainWindow::translate()
{
    setCursor(QCursor(Qt::WaitCursor));
    translateText(sourceLanguage(), resultLanguage());
    setCursor(QCursor());
}

void MainWindow::timerTranslate()
{
    if (applicationInFocus())
        translate();
}

void MainWindow::reverse()
{
    translateText(resultLanguage(), sourceLanguage());
}

void MainWindow::changeVisibility()
{
    setVisible(!isVisible());
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    if(e->type() ==  QEvent::LanguageChange) {
        action_swap->setText(tr("Swap languages"));
        action_settings->setText(tr("Settings"));
        action_languages->setText(tr("Languages"));
        action_about->setText(tr("About"));
        action_exit->setText(tr("Exit"));
        menu_button->setToolTip(tr("Open LiteTran menu"));
        translate_button->setText(tr("Translate"));
        swap_button->setToolTip(tr("Swap languages"));
        source_combobox->setToolTip(tr("Source language"));
        result_combobox->setToolTip(tr("Result language"));

        about_title = tr("About LiteTran");

        QFile file(QString("%1/about/%2.html").arg(APP_I18N_DIR, settings_dialog->language()));
        if (!file.open(QFile::ReadOnly))
            qWarning() << "Cannot open About dialog text file: " << file.errorString();
        about_text = file.readAll();
        about_text.replace("@VERSION@", APP_VERSION);
        file.close();
        source_text->setPlaceholderText(tr("Enter text here..."));
    }
}

bool MainWindow::applicationInFocus()
{
    return (isActiveWindow() || settings_dialog->isActiveWindow() || languages_dialog->isActiveWindow());
}

QString MainWindow::sourceLanguage() const
{
    return source_combobox->currentData().toString();
}

QString MainWindow::resultLanguage() const
{
    return result_combobox->currentData().toString();
}

QString MainWindow::sourceText() const
{
    return source_text->toPlainText();
}

QString MainWindow::resultText() const
{
    return result_text->toPlainText();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef APP_WM_COCOA
    if (event->spontaneous()) {
        /** if event initiated from application (close button clicked) */
        event->ignore();
        ProcessSerialNumber pn;
        // NOTICE: GetFrontProcess and ShowHideProcess are deprecated in OS X 10.9
        GetCurrentProcess(&pn); // gets application process identifier
        ShowHideProcess(&pn, false); // hides application in tray
    } else {
        /**
         * if event initiated outside of application (selected Quit in dock
         * context menu)
         */
        QMainWindow::closeEvent(event);
    }
#else
    QMainWindow::closeEvent(event);
#endif
}

void MainWindow::updateSettings()
{
    if(!settings_dialog->trayIconEnabled() && !applicationInFocus())
        show();

    tray_icon->setVisible(settings_dialog->trayIconEnabled());
    translate_shortcut_global->setShortcut(settings_dialog->translateShortcut());
    translate_shortcut_global->setEnabled(settings_dialog->translateShortcutEnabled());
    reverse_shortcut_global->setShortcut(settings_dialog->reverseShortcut());
    reverse_shortcut_global->setEnabled(settings_dialog->reverseShortcutEnabled());
    translate_engine->setDictionaryEnabled(settings_dialog->dictionaryEnabled());

    if (settings_dialog->autoTranslate())
        connect(source_text, SIGNAL(textChanged()), &translate_timer, SLOT(start()));
    else
        disconnect(source_text, SIGNAL(textChanged()), &translate_timer, SLOT(start()));


    qApp->setQuitOnLastWindowClosed(!settings_dialog->trayIconEnabled());

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
    pronounce_engine->say(resultText().split("\n").first(), resultLanguage());
}

void MainWindow::languageChanged()
{
    QString tooltip = APP_NAME;
    tooltip += "\n\n" + source_combobox->currentText();
    tooltip += " " UNICODE_ARROW " "  + result_combobox->currentText();

    const bool enabled = source_combobox->currentIndex() != 0;
    action_swap->setEnabled(enabled);
    swap_button->setEnabled(enabled);

    tray_icon->setToolTip(tooltip);
}

void MainWindow::updateLanguages()
{
    const QString sl = source_combobox->currentText();
    const QString tl = result_combobox->currentText();

    source_combobox->clear();
    result_combobox->clear();
    source_combobox->addItem(APP_FLAG("auto"), "Auto", "auto");
    LanguageList langs = languages_dialog->languages();
    foreach(Language lang, langs) {
        const QString name = lang.first;
        const QString code = lang.second;
        source_combobox->addItem(APP_FLAG(code), name, code);
        result_combobox->addItem(APP_FLAG(code), name, code);
    }
    source_combobox->setCurrentText(sl);
    result_combobox->setCurrentText(tl);
}
