#ifndef QUADSPLIT_H
#define QUADSPLIT_H

#include <QSplitter>

class QuadSplit : public QSplitter
{
    Q_OBJECT

    QSplitter parentSplit;
    QSplitter childSplit1;
    QSplitter childSplit2;

    QWidget *widget1;
    QWidget *widget2;
    QWidget *widget3;
    QWidget *widget4;

private slots:
    void sync1()
    {
        childSplit2.setSizes(childSplit1.sizes());
    }

    void sync2()
    {
        childSplit1.setSizes(childSplit2.sizes());
    }


public:
    QuadSplit() : widget1(0),widget2(0),widget3(0),widget4(0) {};
    QuadSplit(QWidget *w1, QWidget *w2, QWidget *w3, QWidget *w4);
    void show() {parentSplit.show();}
};

#endif // QUADSPLIT_H
