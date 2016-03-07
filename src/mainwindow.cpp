#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trayicon.h"
#include "settings.h"
#include <QMenu>
#include <QDebug>
#include "models.h"
#include "defines.h"
#include <QItemSelectionModel>
#include <QSettings>
#include <algorithm>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	mTrayIcon(new TrayIcon(this)),
	mComboboxModel(new LanguageComboboxModel(mLanguages, this)),
	mFilter(new LanguageFilter(this)),
	mSettings(new Settings(this))
{
	ui->setupUi(this);
	this->setUnifiedTitleAndToolBarOnMac(true);

	mTranslateTimer.setInterval(1000);
	mTranslateTimer.setSingleShot(true);

	connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
	connect(ui->actionPreferences, &QAction::triggered, mSettings, &QDialog::exec);
	connect(&mEngine, &TranslateEngine::translationArrived, ui->ResultTextBrowser, &QTextBrowser::setText);

	connect(&mTranslateTimer, &QTimer::timeout, [=](){
		if (mSettings->getAutoTranslateEnabled())
			ui->TranslateButton->click();
	});

	connect(ui->SourceTextEdit, &QPlainTextEdit::textChanged, [=](){
		mTranslateTimer.start();
	});

	connect(ui->SourceLanguageCombobox, &QComboBox::currentTextChanged, [=](const QString &){
		mTranslateTimer.start();
	});

	connect(ui->ResultLanguageCombobox, &QComboBox::currentTextChanged, [=](const QString &){
		mTranslateTimer.start();
	});

	connect(ui->SwapButton, &QPushButton::clicked, [=](){
		const int idx = ui->SourceLanguageCombobox->currentIndex();
		ui->SourceLanguageCombobox->setCurrentIndex(ui->ResultLanguageCombobox->currentIndex());
		ui->ResultLanguageCombobox->setCurrentIndex(idx);
	});

	connect(&mEngine, &TranslateEngine::languagesArrived, [=](const LanguageVector &ret){
		mLanguages = ret;
		std::sort(mLanguages.begin(), mLanguages.end(), [=](const Language &l1, const Language &l2) -> bool {
			return l1.name.toLower() < l2.name.toLower();
		});
		readSettings();
		mFilter->setSourceModel(mComboboxModel);
		mSettings->setModel(mComboboxModel);
		ui->SourceLanguageCombobox->setModel(mFilter);
		ui->ResultLanguageCombobox->setModel(mFilter);
		setEnabled(true);
	});

	connect(ui->TranslateButton, &QPushButton::clicked, [=](){
		const QString sl = mapIndexToCode(ui->SourceLanguageCombobox->currentIndex());
		const QString tl = mapIndexToCode(ui->ResultLanguageCombobox->currentIndex());
		mEngine.requestTranslation(sl, tl, ui->SourceTextEdit->toPlainText());
	});

	connect(mSettings, &QDialog::accepted, [=](){
		mComboboxModel->reload();
	});

	mEngine.requestLanguages();
	setDisabled(true);

	QMenu *trayMenu = new QMenu(this);
	trayMenu->addAction(ui->actionPreferences);
	trayMenu->addSeparator();
	trayMenu->addAction(ui->actionQuit);
	trayMenu->addAction(ui->actionAbout);
	mTrayIcon->setContextMenu(trayMenu);
	mTrayIcon->setVisible(true);
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete ui;
}

void MainWindow::readSettings()
{
	QSettings s;
	QStringList enabled = s.value("EnabledLanguages").toString().split(',');
	for(Language &l : mLanguages)
	{
		if (enabled.contains(l.code))
			l.enabled = true;
	}
}

void MainWindow::saveSettings()
{
	QSettings s;
	QStringList enabled;
	for(Language &l : mLanguages)
	{
		if (l.enabled)
			enabled << l.code;
	}
	s.setValue("EnabledLanguages", enabled.join(','));
}

QString MainWindow::mapIndexToCode(const int idx)
{
	return mLanguages.at(mFilter->mapToSource(mFilter->index(idx, 0, QModelIndex())).row()).code;
}
