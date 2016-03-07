#include "defines.h"
#include <QApplication>
#include <QDebug>
#include <QLocalSocket>
#include <QLocalServer>
#include <QCommandLineParser>
#include "mainwindow.h"

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
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG);
    app.setApplicationVersion(APP_VERSION);

//    QCommandLineParser parser;
//    parser.addHelpOption();
//    parser.addVersionOption();
//    parser.setApplicationDescription(APP_NAME);

//    QCommandLineOption opt_collapsed(QStringList() << "c" << "collapsed", "Run collapsed in tray.");
//    parser.addOption(opt_collapsed);

//    parser.process(app);

//    bool collapsed = parser.isSet(opt_collapsed);
//    MainWindow window(collapsed);

	MainWindow window;

	window.show();

	QObject::connect(&srv, &QLocalServer::newConnection, &window, &MainWindow::show);

    return app.exec();
}
