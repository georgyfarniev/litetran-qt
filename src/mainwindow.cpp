#include <QMenu>
#include <QDebug>
#include <QItemSelectionModel>
#include <QSettings>
#include <QKeySequence>
#include <QDebug>
#include <QCursor>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <algorithm>

#include "3rdparty/qxtshortcut/qxtglobalshortcut.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "trayicon.h"
#include "settings.h"
#include "popup.h"

namespace constants
{
    const QString translate_key = "trnsl.1.1.20160222T212917Z.dac5812c38fde523.efb3b5e5d4634845e1a6106e891343e83d1423d2";
    const QString dictionary_key = "dict.1.1.20160223T080804Z.0afde59e8b8ec833.b3ac76f261b74ed73308d67f5e79ffba234f5926";
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	mPopup(new Popup()),
	mTrayIcon(new TrayIcon(this)),
	mComboboxModel(new LanguageComboboxModel(mLanguages, this)),
	mFilter(new LanguageFilter(this)),
    mSettings(new Settings(this)),
	mTranslateShortcut(new QxtGlobalShortcut(this)),
    mAppearShortcut(new QxtGlobalShortcut(this)),
    mState(State::Idle)
{
	ui->setupUi(this);

    mEngine.setTranslateKey(constants::translate_key);
    mEngine.setDictionaryKey(constants::dictionary_key);

	createActionsConnections();
	createTimerConnections();
	createAsyncConnections();

	// Setup model
	mFilter->setSourceModel(mComboboxModel);
	mSettings->setModel(mComboboxModel);
	ui->SourceLanguageCombobox->setModel(mFilter);
	ui->ResultLanguageCombobox->setModel(mFilter);

	// Setup GUI
	setUnifiedTitleAndToolBarOnMac(true);
	createTrayMenu();
	mTrayIcon->setVisible(mSettings->getTrayIconEnabled());
	mTranslateShortcut->setShortcut(mSettings->getPopupShortcut());
	mAppearShortcut->setShortcut(mSettings->getAppShortcut());
	QSettings s;
	s.beginGroup("MainWindow");
	restoreGeometry(s.value("Geometry").toByteArray());
	s.endGroup();

	// Request available languages
	mEngine.requestLanguages();

    mTranslateTimer.setSingleShot(true);
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete ui;
}

void MainWindow::createActionsConnections()
{
	connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionPreferences, &QAction::triggered, mSettings, &Settings::exec);
    connect(mPopup, &Popup::requestShowWindow, [=](){ activateWindow();});
    connect(mTrayIcon, &TrayIcon::clicked, [=](){ setVisible(!isVisible());});

	connect(ui->actionClear, &QAction::triggered, [=]()
	{
		ui->SourceTextEdit->clear();
		ui->ResultTextBrowser->clear();
	});

	connect(mTranslateShortcut, &QxtGlobalShortcut::activated, [=](){
        ui->SourceTextEdit->setPlainText(QApplication::clipboard()->text(QClipboard::Selection));
		ui->TranslateButton->click();
	});

	connect(ui->actionSwap, &QAction::triggered, [=](){
		const int idx = ui->SourceLanguageCombobox->currentIndex();
		ui->SourceLanguageCombobox->setCurrentIndex(ui->ResultLanguageCombobox->currentIndex());
		ui->ResultLanguageCombobox->setCurrentIndex(idx);
	});

	connect(ui->actionAbout, &QAction::triggered, [=](){
		QMessageBox::information(this, tr("About LiteTran"), tr("LiteTran - translate selected text"));
	});

	connect(mAppearShortcut, &QxtGlobalShortcut::activated, [=](){
		ui->actionClear->trigger();
		show();
	});

    connect(ui->TranslateButton, &QPushButton::clicked, [=](){
        if(mState == State::Idle && !ui->SourceTextEdit->toPlainText().isEmpty())
        {
            mEngine.requestTranslation(sourceLanguage().code, resultLanguage().code, ui->SourceTextEdit->toPlainText());
            mState = State::WaitingForTranslate;
        }
    });
}

void MainWindow::createTimerConnections()
{
	// Timer handlers
	connect(&mTranslateTimer, &QTimer::timeout, [=](){
        if (mState == State::Idle && mSettings->getAutoTranslateEnabled() && !ui->SourceTextEdit->toPlainText().isEmpty())
		{
			ui->TranslateButton->click();
			mTranslateTimer.stop();
		}
	});

    connect(ui->SourceTextEdit, &QPlainTextEdit::textChanged, [=](){ mTranslateTimer.start(); });
    connect(ui->SourceLanguageCombobox, &QComboBox::currentTextChanged, [=](const QString &) { mTranslateTimer.start(); });
    connect(ui->ResultLanguageCombobox, &QComboBox::currentTextChanged, [=](const QString &) { mTranslateTimer.start();});
}

void MainWindow::createAsyncConnections()
{
	// Async actions handlers
	connect(&mEngine, &TranslateEngine::languagesArrived, [=](const LanguageVector &ret){
		mLanguages = ret;

		if (mLanguages.isEmpty())
		{
			qWarning() << "Unable to fetch languages!";
			setDisabled(true);
			return;
		}

        readSettings();
	});

    connect(&mEngine, &TranslateEngine::dictionaryArrived, [=](const QString &result){
        mState = State::Idle;

        if (!result.isEmpty())
            ui->ResultTextBrowser->append("<hr>" + result);

        if (!isActiveWindow())
            mPopup->display(sourceLanguage(), resultLanguage(), ui->ResultTextBrowser->toHtml());
    });

    connect(&mEngine, &TranslateEngine::translationArrived, [=](const QStringList &results){
        ui->ResultTextBrowser->setText(results.join(';'));

        if (mSettings->getDictionaryEnabled() && results.size() == 1)
        {
            mEngine.requestDictionary(sourceLanguage().code, resultLanguage().code, ui->SourceTextEdit->toPlainText());
            mState = State::WaitingForDictionary;
            return;
        }

        if (!isActiveWindow())
            mPopup->display(sourceLanguage(), resultLanguage(), ui->ResultTextBrowser->toHtml());

        mState = State::Idle;
    });

    connect(mSettings, &QDialog::accepted, [=](){
        mTrayIcon->setVisible(mSettings->getTrayIconEnabled());
        mTranslateShortcut->setShortcut(mSettings->getPopupShortcut());
        mAppearShortcut->setShortcut(mSettings->getAppShortcut());
        const QString sl = ui->SourceLanguageCombobox->currentText();
        const QString tl = ui->ResultLanguageCombobox->currentText();
        mComboboxModel->reload();
        ui->SourceLanguageCombobox->setCurrentText(sl);
        ui->ResultLanguageCombobox->setCurrentText(tl);

        if (ui->SourceLanguageCombobox->currentIndex() == -1)
            ui->SourceLanguageCombobox->setCurrentIndex(0);

        if (ui->ResultLanguageCombobox->currentIndex() == -1)
            ui->ResultLanguageCombobox->setCurrentIndex(0);

        qApp->setQuitOnLastWindowClosed(!mSettings->getTrayIconEnabled());
    });
}

void MainWindow::createTrayMenu()
{
	//! Tray menu configuration
	QMenu *trayMenu = new QMenu(this);
	trayMenu->addAction(ui->actionPreferences);
	trayMenu->addAction(ui->actionSwap);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionAbout);
	trayMenu->addAction(ui->actionQuit);
	mTrayIcon->setContextMenu(trayMenu);
}

void MainWindow::readSettings()
{
    std::sort(mLanguages.begin(), mLanguages.end(), [=](const Language &l1, const Language &l2) -> bool {
        return l1.name.toLower() < l2.name.toLower();
    });

    QSettings s;
    s.beginGroup("MainWindow");
    const QStringList enabled = s.value("EnabledLanguages").toString().split(',', QString::SkipEmptyParts);

    mLanguages.setEnabledLanguages(enabled);


    mComboboxModel->reload();

    ui->SourceLanguageCombobox->setCurrentText(s.value("SourceLanguage").toString());
    ui->ResultLanguageCombobox->setCurrentText(s.value("ResultLanguage").toString());

    if (ui->SourceLanguageCombobox->currentIndex() == -1)
        ui->SourceLanguageCombobox->setCurrentIndex(0);

    if (ui->ResultLanguageCombobox->currentIndex() == -1)
        ui->ResultLanguageCombobox->setCurrentIndex(0);
    s.endGroup();
}

void MainWindow::saveSettings()
{
	QSettings s;
	s.beginGroup("MainWindow");
    s.setValue("EnabledLanguages", mLanguages.enabledLanguages().join(','));
	s.setValue("SourceLanguage", sourceLanguage().name);
	s.setValue("ResultLanguage", resultLanguage().name);
	s.setValue("Geometry", saveGeometry());
	s.setValue("Visible", isVisible());
	s.endGroup();
}

Language MainWindow::mapIndexToLanguage(const int idx)
{
    return mLanguages.at(mFilter->mapToSource(mFilter->index(idx, 0, QModelIndex())).row());
}

Language MainWindow::sourceLanguage()
{
	return mapIndexToLanguage(ui->SourceLanguageCombobox->currentIndex());
}

Language MainWindow::resultLanguage()
{
	return mapIndexToLanguage(ui->ResultLanguageCombobox->currentIndex());
}

