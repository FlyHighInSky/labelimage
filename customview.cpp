#include "customview.h"

CustomView::CustomView(QObject* parent)
{
//    _cursor = Qt::ArrowCursor;
}

void CustomView::updateCursor(bool checked)
{
    if (checked) {
        _cursor = Qt::CrossCursor;
    } else {
        _cursor = Qt::ArrowCursor;
    }

    bool cursorFlag = false;
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            QPoint viewPoint = mapFromGlobal(QCursor::pos());
            QPointF scenePoint = mapToScene(viewPoint);
            if (item->contains(scenePoint)) {
                cursorFlag = true;
            }
        }
    }
    if (!cursorFlag) {
        QApplication::setOverrideCursor(_cursor);
    }
}

void CustomView::enterEvent(QEvent *event)
{
    if (_isPanning)
        _cursor = Qt::OpenHandCursor;
    QApplication::setOverrideCursor(_cursor);
}

void CustomView::leaveEvent(QEvent *event)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void CustomView::panImage(bool op)
{
    _isPanning = op;
}

void CustomView::mousePressEvent(QMouseEvent *event)
{
    if (_isPanning)
    {
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
    if (_isPanning)
    {
        _startFlag = false;
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CustomView::mouseMoveEvent(QMouseEvent *event)
{
    if (_isPanning && _startFlag)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
        _panStartX = event->x();
        _panStartY = event->y();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}
