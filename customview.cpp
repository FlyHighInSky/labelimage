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
    QApplication::setOverrideCursor(_cursor);
}

void CustomView::leaveEvent(QEvent *event)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}
