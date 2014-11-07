#include "interfaces.h"
#include "paintarea.h"

#include <QPainter>
#include <QMouseEvent>

PaintArea::PaintArea(QWidget *parent) :
    QWidget(parent),
    theImage(400, 300, QImage::Format_RGB32),
    color(Qt::blue),
    thickness(0),
    chartInterface(0),
    lastPos(-1, -1)
{
    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_NoBackground);

    theImage.fill(qRgb(255, 255, 255));
}

bool PaintArea::openImage(const QString &fileName)
{
    QImage image;
    if (!image.load(fileName))
        return false;

    setImage(image);
    return true;
}

bool PaintArea::saveImage(const QString &fileName, const char *fileFormat)
{
    return theImage.save(fileName, fileFormat);
}

void PaintArea::setImage(const QImage &image)
{
    theImage = image.convertToFormat(QImage::Format_RGB32);
    update();
    updateGeometry();
}

void PaintArea::insertShape(const QPainterPath &path)
{
    pendingPath = path;
#ifndef QT_NO_CURSOR
    setCursor(Qt::CrossCursor);
#endif
}

void PaintArea::setChartColor(const QColor &color)
{
    this->color = color;
}

void PaintArea::setChartWidth(int width)
{
    thickness = width;
}

//! [0]
void PaintArea::setChart(ChartInterface *chartInterface, const QString &chart)
{
    this->chartInterface = chartInterface;
    this->chart = chart;
}
//! [0]

QSize PaintArea::sizeHint() const
{
    return theImage.size();
}

void PaintArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), theImage);
}

void PaintArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!pendingPath.isEmpty()) {
            QPainter painter(&theImage);
            setupPainter(painter);

            const QRectF boundingRect = pendingPath.boundingRect();
            QLinearGradient gradient(boundingRect.topRight(),
                                     boundingRect.bottomLeft());
            gradient.setColorAt(0.0, QColor(color.red(), color.green(),
                                            color.blue(), 63));
            gradient.setColorAt(1.0, QColor(color.red(), color.green(),
                                            color.blue(), 191));
            painter.setBrush(gradient);
            painter.translate(event->pos() - boundingRect.center());
            painter.drawPath(pendingPath);

            pendingPath = QPainterPath();
#ifndef QT_NO_CURSOR
            unsetCursor();
#endif
            update();
        } else {
            if (chartInterface) {
                QPainter painter(&theImage);
                setupPainter(painter);
                const QRect rect = chartInterface->mousePress(chart, painter,
                                                              event->pos());
                update(rect);
            }

            lastPos = event->pos();
        }
    }
}

//! [1]
void PaintArea::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && lastPos != QPoint(-1, -1)) {
        if (chartInterface) {
            QPainter painter(&theImage);
            setupPainter(painter);
            const QRect rect = chartInterface->mouseMove(chart, painter, lastPos,
                                                         event->pos());
            update(rect);
        }

        lastPos = event->pos();
    }
}
//! [1]

void PaintArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && lastPos != QPoint(-1, -1)) {
        if (chartInterface) {
            QPainter painter(&theImage);
            setupPainter(painter);
            QRect rect = chartInterface->mouseRelease(chart, painter,
                                                      event->pos());
            update(rect);
        }

        lastPos = QPoint(-1, -1);
    }
}

void PaintArea::setupPainter(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(color, thickness, Qt::SolidLine, Qt::RoundCap,
                   Qt::RoundJoin));
}
