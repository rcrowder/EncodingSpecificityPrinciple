#include "QuadSplit.h"

QuadSplit::QuadSplit(QWidget *w1, QWidget *w2, QWidget *w3, QWidget *w4)
    : widget1(w1),
      widget2(w2),
      widget3(w3),
      widget4(w4)
{
    parentSplit.setOrientation(Qt::Vertical);

    childSplit1.addWidget(w1);
    childSplit1.addWidget(w2);
    childSplit2.addWidget(w3);
    childSplit2.addWidget(w4);

    parentSplit.addWidget(&childSplit1);
    parentSplit.addWidget(&childSplit2);

    connect (&childSplit1,
             SIGNAL(splitterMoved(int,int)),
             this,
             SLOT(sync1()));

    connect (&childSplit2,
             SIGNAL(splitterMoved(int,int)),
             this,
             SLOT(sync2()));
}
