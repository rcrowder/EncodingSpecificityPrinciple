//! [0]
#include <QApplication>
#include <QCommandLineParser>
#include <QtPlugin>
#include <QtQml>


#include "mainwindow.h"

Q_IMPORT_PLUGIN(BasicToolsPlugin)

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGuiApplication::setApplicationDisplayName(MainWindow::tr("Cochlear Auditory Encoding"));

    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(MainWindow::tr("[file]"), MainWindow::tr("Audio file to open."));
    commandLineParser.process(QCoreApplication::arguments());

    MainWindow mainApp;

    qmlRegisterType<MainWindow>("CochlearNerveWindow", 1, 0, "mainApp");

    if (!commandLineParser.positionalArguments().isEmpty()
        && !mainApp.LoadFile(commandLineParser.positionalArguments().front())) {
        return -1;
    }
    mainApp.Start(&app);
    mainApp.show();

    return app.exec();
}
