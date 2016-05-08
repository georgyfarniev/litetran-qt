#include <QMenu>
#include <QDebug>
#include <QItemSelectionModel>
#include <QSettings>
#include <QKeySequence>
#include <QDebug>
#include <QToolTip>
#include <QCursor>
#include <QMessageBox>
#include <algorithm>

#include "3rdparty/qxtshortcut/qxtglobalshortcut.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "trayicon.h"
#include "settings.h"
#include "platform/clipboard.h"
#include "defines.h"
#include "popup.h"

#ifdef APP_WM_COCOA
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#endif

#define TRANSLATE_KEY "trnsl.1.1.20160222T212917Z.dac5812c38fde523.efb3b5e5d4634845e1a6106e891343e83d1423d2"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	mPopup(new Popup()),
	mTrayIcon(new TrayIcon(this)),
	mComboboxModel(new LanguageComboboxModel(mLanguages, this)),
	mFilter(new LanguageFilter(this)),
	mSettings(new Settings(this)),
	mTranslateShortcut(new QxtGlobalShortcut(this)),
	mAppearShortcut(new QxtGlobalShortcut(this))
{
	ui->setupUi(this);

//	mEngine.setTranslateKey(mSettings->getTranslateKey());
	mEngine.setTranslateKey(TRANSLATE_KEY);

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
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete ui;
}

void MainWindow::createActionsConnections()
{
	// Regular actions handlers
	connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
	connect(ui->actionPreferences, &QAction::triggered, mSettings, &QDialog::exec);

	connect(ui->actionClear, &QAction::triggered, [=]()
	{
		ui->SourceTextEdit->clear();
		ui->ResultTextBrowser->clear();
	});

	connect(mTranslateShortcut, &QxtGlobalShortcut::activated, [=](){
		ui->SourceTextEdit->setPlainText(Clipboard::selectedText());
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
		if(!ui->SourceTextEdit->toPlainText().isEmpty())
			mEngine.requestTranslation(sourceLanguage().code, resultLanguage().code, ui->SourceTextEdit->toPlainText());
	});

    connect(mPopup, &Popup::requestShowWindow, [=](){
        activateWindow();
    });
}

void MainWindow::createTimerConnections()
{
	// Timer handlers
	connect(&mTranslateTimer, &QTimer::timeout, [=](){
		if (mSettings->getAutoTranslateEnabled() && !ui->SourceTextEdit->toPlainText().isEmpty())
		{
			ui->TranslateButton->click();
			mTranslateTimer.stop();
		}
	});

	connect(ui->SourceTextEdit, &QPlainTextEdit::textChanged, [=](){
		mTranslateTimer.start();
	});

	connect(ui->SourceLanguageCombobox, &QComboBox::currentTextChanged, [=](const QString &) {
		mTranslateTimer.start();
	});

	connect(ui->ResultLanguageCombobox, &QComboBox::currentTextChanged, [=](const QString &) {
		mTranslateTimer.start();
	});
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

		std::sort(mLanguages.begin(), mLanguages.end(), [=](const Language &l1, const Language &l2) -> bool {
			return l1.name.toLower() < l2.name.toLower();
		});

		QSettings s;
		s.beginGroup("MainWindow");
		const QStringList enabled = s.value("EnabledLanguages").toString().split(',', QString::SkipEmptyParts);


		if (!enabled.isEmpty())
		{
			qDebug() << "Enabled languages: " << enabled;

			for(Language &l : mLanguages)
			{
				if (!enabled.contains(l.code))
					l.enabled = false;
			}
		}

		mComboboxModel->reload();

		ui->SourceLanguageCombobox->setCurrentText(s.value("SourceLanguage", "English").toString());
		ui->ResultLanguageCombobox->setCurrentText(s.value("ResultLanguage", "Russian").toString());
		s.endGroup();
	});

	connect(&mEngine, &TranslateEngine::translationArrived, [=](const QString &result){
		ui->ResultTextBrowser->setText(result);
		if (!isActiveWindow() && !mSettings->isActiveWindow())
		{
			 mPopup->display(sourceLanguage().name, resultLanguage().name, sourceLanguage().code, resultLanguage().code, ui->ResultTextBrowser->toPlainText());
			// Mac version
//			move(QCursor::pos());
//			hide();
//			show();
//			raise();
		}
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
}

void MainWindow::saveSettings()
{
	QSettings s;
	s.beginGroup("MainWindow");
	QStringList enabled;
	for(Language &l : mLanguages)
	{
		if (l.enabled)
			enabled << l.code;
	}
	s.setValue("EnabledLanguages", enabled.join(','));
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
