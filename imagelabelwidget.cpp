#include "imagelabelwidget.h"

ImageLabelWidget::ImageLabelWidget(QWidget *parent)
{
    dragging = 0;
    initFlag = true;
}

void ImageLabelWidget::paintEvent(QPaintEvent *event)
{
    if (initFlag)
        return;

    QPainter paint(this);
    paint.drawImage(QPoint(0, 0), image);
//    setPixmap(QPixmap::fromImage(image));
    paint.setPen(QPen(Qt::blue,5));
    for(int i=0; i<4; i++)
        paint.drawPoint(X[i],Y[i]);
    paint.setPen(Qt::red);
    QPolygon pts;
    pts.setPoints(4, X[0],Y[0], X[1],Y[1], X[2],Y[2],X[3],Y[3]);
    paint.drawConvexPolygon(pts);
}

void ImageLabelWidget::setImage(const QImage &img)
{
    image = img;
    initFlag = false;
    update();
}

void ImageLabelWidget::mouseReleaseEvent(QMouseEvent *event)
{
    dragging = 0;
}

void ImageLabelWidget::mousePressEvent(QMouseEvent *event)
{
    for(int i=0;i<4;i++) {
        if(event->pos().x()==X[i]&&event->pos().y()==Y[i]) {
            dragging = 1;
            j=i;
            break;
        }
        //else continue;
    }
}

void ImageLabelWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging == 1) {
        X[j] = event->pos().x();
        Y[j] = event->pos().y();
        update();
    }
}
