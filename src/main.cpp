#include "mainwindow.h"
#include "defines.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG);

    MainWindow window;
    window.show();

    return app.exec();
}
