#include <QtQml>
#include <QApplication>
#include <QQmlApplicationEngine>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointF>
#include <QVector>

#include "FileOpenHandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    FileOpenHandler _fileOpenHandler;
    engine.rootContext()->setContextProperty("_fileOpenHandler", &_fileOpenHandler);

    QVector <QPointF> points;

    // Fill in points with n number of points
    for(int i = 0; i< 100; i++)
       points.append(QPointF(i*3, i*5));

    // Create a view, put a scene in it and add tiny circles
    // in the scene
    QGraphicsView * view = new QGraphicsView();
    QGraphicsScene * scene = new QGraphicsScene();
    view->setScene(scene);

    for(int i = 0; i< points.size(); i++)
        scene->addEllipse(points[i].x(), points[i].y(), 1, 1);

    // Show the view
    view->show();

    // or add the view to the layout inside another widget

    return app.exec();
}
