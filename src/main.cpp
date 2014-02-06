#include "mainwindow.h"
#include "defines.h"
#include <QApplication>
#include <QDebug>
#include <QLocalSocket>
#include <QLocalServer>

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

    MainWindow window;

    QObject::connect(&srv, SIGNAL(newConnection()), &window, SLOT(show()));

    return app.exec();
}
