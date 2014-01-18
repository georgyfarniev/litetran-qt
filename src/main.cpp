#include "mainwindow.h"
#include "defines.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG);

    QSettings settings;
    settings.beginGroup("MainWindow");

    MainWindow window;
    if(settings.value("Visible", true).toBool())
        window.show();

    return app.exec();
}
