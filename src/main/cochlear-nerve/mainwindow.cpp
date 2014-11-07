#include "interfaces.h"
#include "mainwindow.h"
#include "paintarea.h"
#include "plugindialog.h"

//#include "sndfile.h"

#include <QScrollArea>
#include <QMessageBox>
#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QApplication>

#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>
#include <QQmlEngine>
#include <QQmlContext>
#include <QThread>
#include <QTimer>

MainWindow::MainWindow() :
    paintArea(new PaintArea),
    scrollArea(new QScrollArea)
{
    viewer = new QQuickView();

    setWindowTitle(tr("Cochlear Auditory Encoding"));
}

MainWindow::~MainWindow()
{
    delete viewer;
}

int MainWindow::start(QGuiApplication* app)
{
    //qDebug() << "Start " + QString::number((int)QThread::currentThreadId());
    this->app = app;

    this->viewer->engine()->rootContext()->setContextProperty("MainWindow", this);
    this->viewer->setSource(QUrl("qrc:qml/splash.qml"));

    // And display it
    this->viewer->show();

    return 0;
}


void MainWindow::init()
{
    QObject::connect((QObject*)viewer->engine(), SIGNAL(quit()), this, SLOT(exit()));
    QObject::connect(app, SIGNAL(aboutToQuit()), this, SLOT(exit()));

    QThread::msleep(1000); // 1 second sleep

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(paintArea);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();
    loadPlugins();

//    if (!chartActionGroup->actions().isEmpty())
//        chartActionGroup->actions().first()->trigger();
//    QTimer::singleShot(500, this, SLOT(aboutPlugins()));
}

void MainWindow::exit()
{
    //this->Stop();
    this->app->exit(0);
}

void MainWindow::shutdown()
{
    //this->Stop();
    this->app->exit(1);
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Open File"),
                                                          QDir::currentPath());
    if (!fileName.isEmpty()) {
        if (!paintArea->openImage(fileName)) {
            QMessageBox::information(this,
                                     tr("Cochlear Auditory Encoding"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        paintArea->adjustSize();
    }
}

bool MainWindow::saveAs()
{
    const QString initialPath = QDir::currentPath() + "/untitled.png";

    const QString fileName = QFileDialog::getSaveFileName(this,
                                                          tr("Save As"),
                                                          initialPath);
    if (fileName.isEmpty()) {
        return false;
    } else {
        return paintArea->saveImage(fileName, "png");
    }
}

void MainWindow::chartColor()
{
    const QColor newColor = QColorDialog::getColor(paintArea->chartColor());
    if (newColor.isValid())
        paintArea->setChartColor(newColor);
}

void MainWindow::chartWidth()
{
    bool ok;
    const int newWidth = QInputDialog::getInt(this,
                                              tr("Cochlear Auditory Encoding"),
                                              tr("Select chart width:"),
                                              paintArea->chartWidth(),
                                              1, 50, 1, &ok);
    if (ok)
        paintArea->setChartWidth(newWidth);
}

//! [0]
void MainWindow::changeChart()
{
    QAction *action = qobject_cast<QAction *>(sender());
    ChartInterface *iChart = qobject_cast<ChartInterface *>(action->parent());
    const QString chart = action->text();

    paintArea->setChart(iChart, chart);
}
//! [0]

//! [1]
void MainWindow::insertShape()
{
    QAction *action = qobject_cast<QAction *>(sender());
    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(action->parent());

    const QPainterPath path = iShape->generateShape(action->text(),
                                                    this);
    if (!path.isEmpty())
        paintArea->insertShape(path);
}
//! [1]

//! [2]
void MainWindow::applyFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());
    FilterInterface *iFilter =
            qobject_cast<FilterInterface *>(action->parent());

    const QImage image = iFilter->filterImage(action->text(),
                                              paintArea->image(),
                                              this);
    paintArea->setImage(image);
}
//! [2]

void MainWindow::about()
{
   QMessageBox::about(this,
                      tr("About Cochlear Auditory Encoding"),
                      tr("The <b>Cochlear Auditory Encoding</b> ..."));
}

//! [3]
void MainWindow::aboutPlugins()
{
    PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
    dialog.exec();
}
//! [3]

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAsAct = new QAction(tr("&Save As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    chartColorAct = new QAction(tr("&Chart Color..."), this);
    connect(chartColorAct, SIGNAL(triggered()), this, SLOT(chartColor()));

    chartWidthAct = new QAction(tr("&Chart Width..."), this);
    connect(chartWidthAct, SIGNAL(triggered()), this, SLOT(chartWidth()));

    chartActionGroup = new QActionGroup(this);

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutPluginsAct = new QAction(tr("About &Plugins"), this);
    connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    chartMenu = menuBar()->addMenu(tr("&Chart"));
    chartMenu->addAction(chartColorAct);
    chartMenu->addAction(chartWidthAct);
    chartMenu->addSeparator();

    shapesMenu = menuBar()->addMenu(tr("&Shapes"));

    filterMenu = menuBar()->addMenu(tr("&Filter"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutPluginsAct);
}

//! [4]
void MainWindow::loadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
        populateMenus(plugin);
//! [4] //! [5]

    pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
//! [5]

//! [6]
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            populateMenus(plugin);
            pluginFileNames += fileName;
//! [6] //! [7]
        }
//! [7] //! [8]
    }
//! [8]

//! [9]
    chartMenu->setEnabled(!chartActionGroup->actions().isEmpty());
    shapesMenu->setEnabled(!shapesMenu->actions().isEmpty());
    filterMenu->setEnabled(!filterMenu->actions().isEmpty());
}
//! [9]

//! [10]
void MainWindow::populateMenus(QObject *plugin)
{
    ChartInterface *iChart = qobject_cast<ChartInterface *>(plugin);
    if (iChart)
        addToMenu(plugin, iChart->chartes(), chartMenu, SLOT(changeChart()),
                  chartActionGroup);

    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
    if (iShape)
        addToMenu(plugin, iShape->shapes(), shapesMenu, SLOT(insertShape()));

    FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
    if (iFilter)
        addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));
}
//! [10]

void MainWindow::addToMenu(QObject *plugin, const QStringList &texts,
                           QMenu *menu, const char *member,
                           QActionGroup *actionGroup)
{
    foreach (QString text, texts) {
        QAction *action = new QAction(text, plugin);
        connect(action, SIGNAL(triggered()), this, member);
        menu->addAction(action);

        if (actionGroup) {
            action->setCheckable(true);
            actionGroup->addAction(action);
        }
    }
}
