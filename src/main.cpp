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
    socket.connectToServer(APP_NAME_FULL);

    if (socket.waitForConnected(500))
        return 0;

    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG);

    MainWindow window;

    QLocalServer::removeServer(APP_NAME_FULL);
    QLocalServer srv;
    QObject::connect(&srv, SIGNAL(newConnection()), &window, SLOT(show()));
    srv.listen(APP_NAME_FULL);

    return app.exec();
}
