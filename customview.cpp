#include "customview.h"

CustomView::CustomView(QObject* parent)
{
    //    _cursor = Qt::ArrowCursor;
}

void CustomView::drawBoxItem(bool checked)
{
    if (checked) {
        _cursor = Qt::CrossCursor;
    } else {
        _cursor = Qt::ArrowCursor;
    }

    bool cursorFlag = false;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            qgraphicsitem_cast<BoxItem *>(item)->setOldCursor(_cursor);
            if (item->isSelected()) {
                QPoint viewPoint = mapFromGlobal(QCursor::pos());
                QPointF scenePoint = mapToScene(viewPoint);
                if (item->contains(scenePoint)) {
                    cursorFlag = true;
                    break;
                }
            }
        }
    }

    if (!cursorFlag) {
        QApplication::setOverrideCursor(_cursor);
    }
}

void CustomView::panImage(bool checked)
{
    _isPanning = checked;
    if (checked) {
        _cursor = Qt::OpenHandCursor;
    } else {
        _cursor = Qt::ArrowCursor;
        foreach (QGraphicsItem *item, scene()->items()) {
            if (item->type() == QGraphicsItem::UserType+1) {
                qgraphicsitem_cast<BoxItem *>(item)->setOldCursor(_cursor);
            }
        }
    }
    QApplication::setOverrideCursor(_cursor);
}

void CustomView::enterEvent(QEvent *event)
{
    QApplication::setOverrideCursor(_cursor);
}

void CustomView::leaveEvent(QEvent *event)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void CustomView::mousePressEvent(QMouseEvent *event)
{
    if (_isPanning) {
        _startFlag = true;
        _panStartX = event->x();
        _panStartY = event->y();
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void CustomView::mouseReleaseEvent(QMouseEvent *event)
{
    if (_isPanning) {
        _startFlag = false;
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CustomView::mouseMoveEvent(QMouseEvent *event)
{
    if (_isPanning && _startFlag) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
        _panStartX = event->x();
        _panStartY = event->y();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

//void CustomView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio)
//{
//    if (!scene() || rect.isNull())
//        return;
//    auto unity = transform().mapRect(QRectF(0, 0, 1, 1));
//    if (unity.isEmpty())
//        return;
//    scale(1/unity.width(), 1/unity.height());
//    auto viewRect = viewport()->rect();
//    if (viewRect.isEmpty())
//        return;
//    auto sceneRect = transform().mapRect(rect);
//    if (sceneRect.isEmpty())
//        return;
//    qreal xratio = viewRect.width() / sceneRect.width();
//    qreal yratio = viewRect.height() / sceneRect.height();

//    // Respect the aspect ratio mode.
//    switch (aspectRatioMode) {
//    case Qt::KeepAspectRatio:
//        xratio = yratio = qMin(xratio, yratio);
//        break;
//    case Qt::KeepAspectRatioByExpanding:
//        xratio = yratio = qMax(xratio, yratio);
//        break;
//    case Qt::IgnoreAspectRatio:
//        break;
//    }
//    scale(xratio, yratio);
//    centerOn(rect.center());
//}
