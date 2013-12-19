#include "mainwindow.h"
#include "defines.h"
#include <QIcon>
#include <QApplication>

#include <QDebug>
#include <QStringRef>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG);

    MainWindow w;
    w.show();

    QString str = "text long very";

//    qDebug() << str.remove(5, 4);



    qDebug() << str.mid(5, 4);

    return app.exec();
}
