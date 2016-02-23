#include "mainwindow.h"
#include "settings.h"
#include "trayicon.h"
#include "translate.h"
#include "popup.h"
#include "pronounce.h"
#include "languagedb.h"
#include "platform/clipboard.h"
#include "defines.h"
#include "textedit.h"
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
#include <QToolBar>

#ifdef APP_WM_COCOA
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#endif

#define DEFAULT_SOURCE_LANGUAGE "English"
#define DEFAULT_RESULT_LANGUAGE "Russian"

MainWindow::MainWindow(bool collapsed, QWidget *parent) :
    QMainWindow(parent),
	source_text(new QTextEdit(this)),
	result_text(new QTextEdit(this)),
    source_combobox(new QComboBox(this)),
    result_combobox(new QComboBox(this)),
    translate_button(new QPushButton(this)),
	swap_button(new QToolButton(this)),
    action_copy(new QAction(this)),
    action_pronounce(new QAction(this)),
    action_swap(new QAction(this)),
    action_settings(new QAction(this)),
    action_languages(new QAction(this)),
    action_about(new QAction(this)),
	action_exit(new QAction(this)),
	toolbar(new QToolBar(this)),
    menu_tray(new QMenu(this)),
    settings(new QSettings(this)),
    ui_translator(NULL),
    translate_shortcut(new QShortcut(this)),
    shortcut_translate(new QxtGlobalShortcut(this)),
    shortcut_appear(new QxtGlobalShortcut(this)),
    clipboard(new Clipboard(this)),
    settings_dialog(new Settings(this)),
    languages_dialog(new Languages(this)),
    tray_icon(new TrayIcon(this)),
    translate_engine(new Translate(this)),
    pronounce_engine(new Pronounce(translate_engine, this)),
    popup(new Popup(this))
{
#ifdef APP_WM_COCOA
	setUnifiedTitleAndToolBarOnMac(true);
	swap_button->setMaximumWidth(22);
	swap_button->setMaximumHeight(22);
//    swap_button->setStyle(new QCommonStyle());
//    swap_button->setStyleSheet("border: 1px solid transparent");
#endif
    setWindowTitle(APP_NAME);
    setWindowIcon(APP_ICON("litetran"));
	toolbar->setMovable(false);
    swap_button->setIcon(APP_ICON("view-refresh"));

    action_copy->setIcon(APP_ICON("edit-copy"));
    action_pronounce->setIcon(APP_ICON("media-playback-start"));
    translate_button->setIcon(APP_ICON("go-next"));
    action_swap->setIcon(APP_ICON("view-refresh"));
    action_settings->setIcon(APP_ICON("preferences-system"));
    action_languages->setIcon(APP_ICON("preferences-desktop-locale"));
    action_about->setIcon(APP_ICON("help-about"));
    action_exit->setIcon(APP_ICON("application-exit"));

    action_exit->setShortcut(QKeySequence("Ctrl+Q"));
    translate_shortcut->setKey(QKeySequence("Ctrl+T"));
    translate_shortcut->setContext(Qt::ApplicationShortcut);
    swap_button->setShortcut(QKeySequence("Ctrl+Shift+S"));

	toolbar->addAction(action_settings);
	toolbar->addAction(action_languages);
	toolbar->addAction(action_about);
	toolbar->addAction(action_exit);

	toolbar->setIconSize(QSize(22, 22));

	addToolBar(toolbar);

    QHBoxLayout *middle_layout = new QHBoxLayout;
    middle_layout->addWidget(source_combobox);
	middle_layout->addWidget(swap_button);
	middle_layout->addWidget(result_combobox);
    middle_layout->addWidget(translate_button);


    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(source_text);
	main_layout->addLayout(middle_layout);
    main_layout->addWidget(result_text);

#ifdef APP_WM_COCOA
	main_layout->setContentsMargins(5, 5, 5, 5);
#endif


    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(main_layout);

    result_text->setReadOnly(true);

    mLockAutoTranslation = true;

    connect(action_swap, &QAction::triggered, this, &MainWindow::swap);
    connect(action_settings, &QAction::triggered, settings_dialog, &Settings::exec);
    connect(action_languages, &QAction::triggered, languages_dialog, &Languages::exec);
    connect(action_exit, &QAction::triggered, this, &MainWindow::quit);
    connect(action_about, &QAction::triggered, this, &MainWindow::about);
    connect(translate_shortcut, &QShortcut::activated, translate_button, &QPushButton::click);
    connect(settings_dialog, &QDialog::accepted, this, &MainWindow::updateSettings);
    connect(languages_dialog, &QDialog::accepted, this, &MainWindow::updateLanguages);
    connect(action_pronounce, &QAction::triggered, this, &MainWindow::pronounce);
    connect(action_copy, &QAction::triggered, this, &MainWindow::copy);
    connect(translate_button, &QPushButton::clicked, this, &MainWindow::translate);
    connect(swap_button, &QPushButton::clicked, this, &MainWindow::swap);
    connect(shortcut_translate, &QxtGlobalShortcut::activated, this, &MainWindow::popupTranslate);
    connect(shortcut_appear, &QxtGlobalShortcut::activated, this, &MainWindow::appearClean);
//    connect(popup, &Popup::pronounceRequested, this, &MainWindow::pronounce);
    connect(popup, &Popup::appearRequested, this, &MainWindow::appear);
    connect(&translate_timer, SIGNAL(timeout()), this, SLOT(translate()));

    translate_timer.setSingleShot(true);
    translate_timer.setInterval(500);

    menu_tray->addAction(action_settings);
    menu_tray->addSeparator();
    menu_tray->addAction(action_swap);
    menu_tray->addSeparator();
    menu_tray->addAction(action_about);
    menu_tray->addAction(action_exit);
    tray_icon->setContextMenu(menu_tray);

//    source_text->addAction(action_copy);
//    source_text->addAction(action_pronounce);
//    result_text->addAction(action_copy);
//    result_text->addAction(action_pronounce);
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
    languageChanged();

    connect(source_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));
    connect(result_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(languageChanged()));

    mLockAutoTranslation = false;
}

MainWindow::~MainWindow()
{
    settings->setValue("SourceLanguage", source_combobox->currentText());
    settings->setValue("ResultLanguage", result_combobox->currentText());
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("Visible", isVisible());
}

void MainWindow::appear()
{
    hide();
    show();
    activateWindow();
    source_text->setFocus();
}

void MainWindow::appearClean()
{
    source_text->clear();
    result_text->clear();
    appear();
}

void MainWindow::about()
{
    QMessageBox::about(this, about_title, about_text);
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::swap()
{
    const int idx = source_combobox->currentIndex();
    source_combobox->setCurrentIndex(result_combobox->currentIndex()  + 1);
    result_combobox->setCurrentIndex(idx - 1);
}

void MainWindow::translate()
{
    const QString text = sourceText();
    if (text.isEmpty())
        return;
    setCursor(QCursor(Qt::WaitCursor));
    const QString result = translate_engine->translate(text, sourceLanguage(), resultLanguage());
    result_text->setHtml(result);
    setCursor(QCursor());
}

void MainWindow::popupTranslate()
{
    if (applicationInFocus())
        return;

    popup->recordCursorPosition();
    source_text->setPlainText(clipboard->selectedText());
    translate();
    const QString result = result_text->toHtml();

    if (!result.isEmpty()) {
        popup->display(source_combobox->currentText(),
                       result_combobox->currentText(),
                       sourceLanguage(),
                       resultLanguage(),
                       result);
    }
}

void MainWindow::changeVisibility()
{
    source_text->setFocus();
    setVisible(!isVisible());
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    if(e->type() ==  QEvent::LanguageChange) {
        action_copy->setText(tr("Copy"));
        action_pronounce->setText(tr("Pronounce"));
        action_swap->setText(tr("Swap languages"));
        action_settings->setText(tr("Settings"));
        action_languages->setText(tr("Languages"));
        action_about->setText(tr("About"));
        action_exit->setText(tr("Exit"));
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
    shortcut_translate->setShortcut(settings_dialog->translateShortcut());
    shortcut_translate->setEnabled(settings_dialog->translateShortcutEnabled());
    shortcut_appear->setShortcut(settings_dialog->appearShortcut());
    shortcut_appear->setEnabled(settings_dialog->appearShortcutEnabled());
    translate_engine->setDictionaryEnabled(settings_dialog->dictionaryEnabled());

    if (settings_dialog->autoTranslate())
        connect(source_text, SIGNAL(textChanged()), &translate_timer, SLOT(start()));
    else
        disconnect(source_text, SIGNAL(textChanged()), &translate_timer, SLOT(start()));

    QApplication::setQuitOnLastWindowClosed(!settings_dialog->trayIconEnabled());

    const QString locale = settings_dialog->language();

//     retranslate only if language changed
    if(locale != last_locale) {
        if(ui_translator != NULL) {
            QApplication::removeTranslator(ui_translator);
            delete ui_translator;
        }

        ui_translator = new QTranslator();

        if(!ui_translator->load(locale + ".qm", APP_I18N_DIR)) {
            qWarning() << "Cannot load translation for language " << locale;
            return;
        }

        if(!QApplication::installTranslator(ui_translator)) {
            qWarning() << "Cannot install translator for language " << locale;
            return;
        }

        last_locale = locale;
    }
}

void MainWindow::copy()
{
    if (source_text->underMouse())
        source_text->copy();
    else
        result_text->copy();
}

void MainWindow::pronounce()
{
    if (source_text->underMouse())
        pronounce_engine->say(sourceText(), sourceLanguage());
    else
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

    if (settings_dialog->autoTranslate() && !mLockAutoTranslation)
        translate_timer.start();
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
