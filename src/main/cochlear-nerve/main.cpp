#include <QtQml>
#include <QTextEdit>
#include <QtQuick/QQuickView>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtGui/QGuiApplication>

#include "FileOpenHandler.h"
#include "QuadSplit.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<QuadSplit>("Charts", 1, 0, "QuadSplit");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    FileOpenHandler _fileOpenHandler;
    engine.rootContext()->setContextProperty("_fileOpenHandler", &_fileOpenHandler);

    QTextEdit *t1 = new QTextEdit;
    QTextEdit *t2 = new QTextEdit;
    QTextEdit *t3 = new QTextEdit;
    QTextEdit *t4 = new QTextEdit;

    QuadSplit quadSplit(t1, t2, t3, t4);
    engine.setParent(&quadSplit);
    //quadSplit.show();

    return app.exec();
}
