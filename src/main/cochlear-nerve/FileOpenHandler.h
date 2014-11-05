#ifndef FILEOPENHANDLER_H
#define FILEOPENHANDLER_H

#include <QtQml>

class FileOpenHandler : public QObject
{
    Q_OBJECT

public:
    explicit FileOpenHandler(QObject *parent = 0) : QObject(parent) {}
    //FileOpenHandler(QObject *parent = 0) : QObject(parent) {}

public slots:
    void buttonClicked();
};

#endif // FILEOPENHANDLER_H
