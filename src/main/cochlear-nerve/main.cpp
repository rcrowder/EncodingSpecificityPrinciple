//! [0]
#include <QApplication>
#include <QtPlugin>
#include <QtQml>


#include "mainwindow.h"

Q_IMPORT_PLUGIN(BasicToolsPlugin)

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<MainWindow>("CochlearNerveWindow", 1, 0, "mainApp");
    MainWindow mainApp;
    mainApp.start(&app);

    return app.exec();
}
