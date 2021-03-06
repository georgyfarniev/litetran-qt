#include <QApplication>
#include <QDebug>
#include <QLocalSocket>
#include <QLocalServer>
#include <QCommandLineParser>
#include <QTranslator>
#include "mainwindow.h"

#define APP_LOCAL_SERVER_NAME "litetran_singleapplication.lock"
#define APP_I18N_DIR (QString(PREFIX) + QString("/share/litetran/i18n"))

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLocalSocket socket;
    socket.connectToServer(APP_LOCAL_SERVER_NAME);

    if (socket.waitForConnected(500)) {
        qWarning() << "Application already running!";
        return 0;
    }

    QLocalServer::removeServer(APP_LOCAL_SERVER_NAME);
    QLocalServer srv;
    srv.listen(APP_LOCAL_SERVER_NAME);

    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_NAME);
    app.setOrganizationDomain(APP_NAME);
    app.setApplicationVersion(APP_VERSION);


	QTranslator translator;

    if (translator.load(QLocale(), QLatin1String(""), QLatin1String(""), APP_I18N_DIR))
		app.installTranslator(&translator);
	else
		qWarning() << "Unable to load locale!";

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.setApplicationDescription(APP_NAME);

	QCommandLineOption opt_collapsed(QStringList() << "c" << "collapsed", "Run collapsed in tray.");
	parser.addOption(opt_collapsed);

	parser.process(app);

	MainWindow window;

	if (!parser.isSet(opt_collapsed))
	{
		window.show();
		QObject::connect(&srv, &QLocalServer::newConnection, &window, &MainWindow::show);
	}

    return app.exec();
}
