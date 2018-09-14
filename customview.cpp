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
