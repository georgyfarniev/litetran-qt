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
    pronounce_engine(new Pronounce(this)),
    popup(new Popup(pronounce_engine, this))
{
#ifdef Q_OS_MAC
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

    connect(action_swap, SIGNAL(triggered()), this, SLOT(swap()));
    connect(action_settings, SIGNAL(triggered()), settings_dialog, SLOT(exec()));
    connect(action_languages, SIGNAL(triggered()), languages_dialog, SLOT(exec()));
    connect(action_exit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(action_about, SIGNAL(triggered()), this, SLOT(about()));
    connect(translate_shortcut, SIGNAL(activated()), translate_button, SLOT(click()));
    connect(settings_dialog, SIGNAL(accepted()), this, SLOT(updateSettings()));
    connect(languages_dialog, SIGNAL(accepted()), this, SLOT(updateLanguages()));
    connect(toolbar_source_text, SIGNAL(requestCopy()), source_text, SLOT(copyAll()));
    connect(toolbar_result_text, SIGNAL(requestCopy()), result_text, SLOT(copyAll()));
    connect(toolbar_source_text, SIGNAL(requestPronounce()), this, SLOT(pronounceSourceText()));
    connect(toolbar_result_text, SIGNAL(requestPronounce()), this, SLOT(pronounceResultText()));
    connect(translate_button, SIGNAL(pressed()), this, SLOT(translate()));
    connect(swap_button, SIGNAL(clicked()), this, SLOT(swap()));
    connect(source_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(result_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(translate_shortcut_global, SIGNAL(activated()), this, SLOT(translate()));
    connect(reverse_shortcut_global, SIGNAL(activated()), this, SLOT(reverse()));

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
    source_combobox->setCurrentIndex(result_combobox->currentIndex());
    result_combobox->setCurrentIndex(idx);
}

void MainWindow::translateText(const QString &sl, const QString &tl)
{
//    popup->freezeCursorPosition();

    if(!applicationInFocus())
        source_text->setPlainText(clipboard->selectedText());

    const QString result = translate_engine->translate(sourceText(), sl, tl);
    result_text->setHtml(result);

    if(!applicationInFocus())
        popup->show(tl, result);
}

void MainWindow::translate()
{
    qDebug() << applicationInFocus();
    setCursor(QCursor(Qt::WaitCursor));
    translateText(sourceLanguage(), resultLanguage());
    setCursor(QCursor());
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
        action_settings->setText(tr("Configure"));
        action_languages->setText(tr("Languages"));
        action_about->setText(tr("About"));
        action_exit->setText(tr("Exit"));
        menu_button->setToolTip(tr("Open LiteTran menu"));
        translate_button->setText(tr("Translate"));
        translate_button->setToolTip(tr("Translate"));
        swap_button->setToolTip(tr("Swap languages"));
        source_combobox->setToolTip(tr("Source language"));
        result_combobox->setToolTip(tr("Result language"));

        about_title = tr("About LiteTran");
        about_text = tr("LiteTran is a lightweight text translation program."
                        "Just select some text, press hotkey and get translation in small popup!"
                        "\n\nLiteTran is licensed under GPL3\n"
                        "Homepage: https://github.com/flareguner/litetran\n"
                        "\nAuthors:\n"
                        "flareguner - author <flarguner@gmail.com>\n"
                        "Yurij Mikhalevich - Mac OS port and testing <0@39.yt>\n"
                        "Uri Herrera, Nitrux S.A. - Icon and logo <contact@nitrux.in>"
                        "\n\nVersion: ");
        about_text += APP_VERSION;
        source_text->setPlaceholderText(tr("Enter text to translate here..."));
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
    tooltip += " - " + result_combobox->currentText();

    tray_icon->setToolTip(tooltip);
}

void MainWindow::updateLanguages()
{
    const QString sl = source_combobox->currentText();
    const QString tl = result_combobox->currentText();

    source_combobox->clear();
    result_combobox->clear();
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
