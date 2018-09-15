#include "boxitem.h"
#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

BoxItem::BoxItem(QRectF sceneRect, QSize imageSize, QStringList &targetTypeNameList, QString targetTypeName):
    _typeNameList(targetTypeNameList),
    _textRect(),
    _textName(),
    _typeName(targetTypeName),
    _color(Qt::red),
    _dragStart(0,0),
    _dragEnd(0,0),
    _sceneRect(sceneRect),
    _grabberWidth(12),
    _grabberHeight(12),
    _imageSize(imageSize)
{
    _textRect.setDefaultTextColor(QColor(255,255,255,255));
    _textRect.setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _textRect.setParentItem(this);
    _textName.setDefaultTextColor(QColor(255,255,255,255));
    _textName.setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _textName.setParentItem(this);
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    this->setAcceptHoverEvents(true);
    _oldCursor = Qt::ArrowCursor;
    initContextMenu();
}

void BoxItem::setTopmost()
{
    QList<QGraphicsItem *> list = collidingItems(Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem *it, list) {
        if (it->type() == QGraphicsItem::UserType + 1) {
            it->stackBefore(this);
        }
    }
}

void BoxItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    switch (event->buttons()) {
    case Qt::LeftButton:
        if (this->isSelected()) {
            setTopmost();
            _selectedGrabber = getSelectedGrabber(event->pos());
            setGrabberCursor(_selectedGrabber);

            _dragStart = event->pos();
            if (_selectedGrabber != BoxRegion) { // grabber is selected
                _taskStatus = Stretching;
            } else if (_rect.contains(event->pos())) { // box is selected
                _taskStatus = Moving;
            }
            // emit the selected box real rect to scene and statusbar.
            emit boxSelected(getRealRect(), _typeName);
        }
        break;
    case Qt::RightButton:
        this->setSelected(true);
        break;
    default:
        break;
    }
    _oldRect = _rect;
}

void BoxItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    QRectF rect;
    event->setAccepted(true);
    if (this->isSelected()) {
        if (_isMouseMoved) {
            switch (_taskStatus) {
            case Moving:
                _dragEnd = event->pos();
                rect = calculateMoveRect(_dragStart, _dragEnd);
                emit moveCompleted(rect, _oldRect);
                break;
            case Stretching:
                _dragEnd = event->pos();
                rect = calculateStretchRect(_dragStart, _dragEnd);
                emit stretchCompleted(rect, _oldRect);
                break;
            default:
                break;
            }
            _taskStatus = Waiting;
        }
        _isMouseMoved = false;
    } else {
        this->setSelected(true);
    }
}

void BoxItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    if (this->isSelected()) {
        _isMouseMoved = true;
        switch (_taskStatus) {
        case Moving:
            this->setRect(calculateMoveRect(_dragStart, event->pos()));
            break;
        case Stretching:
            this->setRect(calculateStretchRect(_dragStart, event->pos()));
            break;
        case Waiting:
            _selectedGrabber = getSelectedGrabber(event->pos());
            setGrabberCursor(_selectedGrabber);
        default:
            break;
        }
    }
}

void BoxItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent *event )
{
    event->setAccepted(true);
    if (this->isSelected()){
        QApplication::setOverrideCursor(_oldCursor);
    }
}

void BoxItem::hoverEnterEvent ( QGraphicsSceneHoverEvent *event )
{
    event->setAccepted(true);
    if (this->isSelected()){
        _selectedGrabber = getSelectedGrabber(event->pos());
        setGrabberCursor(_selectedGrabber);
    }
}

void BoxItem::hoverMoveEvent ( QGraphicsSceneHoverEvent *event )
{
    event->setAccepted(true);
    if (this->isSelected()){
        _selectedGrabber = getSelectedGrabber(event->pos());
        setGrabberCursor(_selectedGrabber);
    }
}

QRectF BoxItem::calculateMoveRect(QPointF dragStart, QPointF dragEnd)
{
    qreal x = dragEnd.x() - dragStart.x() + _oldRect.left();
    qreal y = dragEnd.y() - dragStart.y() + _oldRect.top();

    if (x <= _sceneRect.left()) {
        x = _sceneRect.left();
    }
    if (y <= _sceneRect.top()) {
        y = _sceneRect.top();
    }
    if (_sceneRect.right()-x <= _rect.width()) {
        x = _sceneRect.right() - _rect.width();
    }

    if (_sceneRect.bottom()-y <= _rect.height()) {
        y = _sceneRect.bottom() - _rect.height();
    }

    return QRectF(x, y, _rect.width(), _rect.height());
}


QRectF BoxItem::calculateStretchRect(QPointF dragStart, QPointF dragEnd)
{
    qreal dx = dragEnd.x() - dragStart.x();
    qreal dy = dragEnd.y() - dragStart.y();

    qreal left = _oldRect.left(), top = _oldRect.top();
    qreal right = _oldRect.right(), bottom = _oldRect.bottom();
    qreal newLeft=left, newTop=top, newRight=right, newBottom=bottom;

    switch(_selectedGrabber) {
    case TopLeft:
        newLeft = qMin(left+dx, right);
        newTop = qMin(top+dy, bottom);
        newRight = qMax(left+dx, right);
        newBottom = qMax(top+dy, bottom);
        break;
    case TopCenter:
        newTop = qMin(top+dy, bottom);
        newBottom = qMax(top+dy, bottom);
        break;
    case TopRight:
        newLeft = qMin(left, right+dx);
        newRight = qMax(left, right+dx);
        newTop = qMin(top+dy, bottom);
        newBottom = qMax(top+dy, bottom);
        break;
    case RightCenter:
        newLeft = qMin(left, right+dx);
        newRight = qMax(left, right+dx);
        break;
    case BottomRight:
        newLeft = qMin(left, right+dx);
        newRight = qMax(left, right+dx);
        newTop = qMin(top, bottom+dy);
        newBottom = qMax(top, bottom+dy);
        break;
    case BottomCenter:
        newTop = qMin(top, bottom+dy);
        newBottom = qMax(top, bottom+dy);
        break;
    case BottomLeft:
        newLeft = qMin(left+dx, right);
        newRight = qMax(left+dx, right);
        newTop = qMin(top, bottom+dy);
        newBottom = qMax(top, bottom+dy);
        break;
    case LeftCenter:
        newLeft = qMin(left+dx, right);
        newRight = qMax(left+dx, right);
        break;
    }

    return QRectF(newLeft, newTop, newRight-newLeft, newBottom-newTop);
}

void BoxItem::setRect(const qreal x, qreal y, qreal w, qreal h)
{
    setRect(QRectF(x, y, w, h));
}

void BoxItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    _boundingRect.setRect(rect.left()-_grabberWidth/2, rect.top()-_grabberHeight/2,
                          rect.width()+_grabberWidth, rect.height()+_grabberHeight);

    _rect = rect;
    if (_sceneRect.intersects(_boundingRect)) {
        if (!_sceneRect.contains(_boundingRect)) {
            _boundingRect = _sceneRect.intersected(_boundingRect);
            _rect = rect.intersected(_boundingRect);
        }
    }
    setGrabbers(_grabberWidth, _grabberHeight);

    qreal halfpw = (_pen.style() == Qt::NoPen) ? qreal(0) : _pen.widthF() / 2;
    if (halfpw > 0.0)
        _boundingRect.adjust(-halfpw, -halfpw, halfpw, halfpw);

    // emit the selected box real rect to scene and statusbar.
    emit boxSelected(getRealRect(), _typeName);

    this->update();
}

QRect BoxItem::getRealRect()
{
    qreal _xScale = _imageSize.width()*1.0/_sceneRect.width();
    qreal _yScale = _imageSize.height()*1.0/_sceneRect.height();
    QRect r((int)(_rect.left()*_xScale), (int)(_rect.top()*_yScale),
            (int)(_rect.width()*_xScale), (int)(_rect.height()*_yScale));

    return r;
}

void BoxItem::setTypeName(QString name)
{
    _typeName = name;
    emit boxSelected(getRealRect(), _typeName);
    this->update();
}

QRectF BoxItem::boundingRect() const
{
    return _boundingRect;
}

void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    double xScale = scale()/painter->transform().m11();
    _pen.setWidthF(_penWidth*xScale);
    _pen.setColor(_color);
    _pen.setStyle(Qt::SolidLine);
    painter->setPen(_pen);

    QBrush brush(QColor(255,0,0,0), Qt::SolidPattern);
    painter->setBrush(brush);

    painter->drawRect(_rect);

    double yScale = scale()/painter->transform().m22();
    if (option->state & QStyle::State_Selected) {
        setGrabbers(_grabberWidth*xScale, _grabberHeight*yScale);
        for (int i=TopLeft; i<=LeftCenter; i++) {
            painter->fillRect(_grabbers[i], QColor(255, 0, 0, 255));
        }
    }

    _textRect.setPos(_rect.topLeft());
    QRect r = getRealRect();
    QString rectInfo = QString("[%1,%2,%3,%4]")
            .arg(r.left()).arg(r.top())
            .arg(r.width()).arg(r.height());
    _textRect.setPlainText(rectInfo);

    _textName.setPos(_rect.bottomLeft());
    _textName.setPlainText(_typeName);
}

GrabberID BoxItem::getSelectedGrabber(QPointF point)
{
    GrabberID ID = BoxRegion;
    if (_grabbers[TopLeft].contains(point)) {
        ID = TopLeft;
    } else if (_grabbers[TopCenter].contains(point)) {
        ID = TopCenter;
    } else if (_grabbers[TopRight].contains(point)) {
        ID = TopRight;
    } else if (_grabbers[RightCenter].contains(point)) {
        ID = RightCenter;
    } else if (_grabbers[BottomRight].contains(point)) {
        ID = BottomRight;
    } else if (_grabbers[BottomCenter].contains(point)) {
        ID = BottomCenter;
    } else if (_grabbers[BottomLeft].contains(point)) {
        ID = BottomLeft;
    } else if (_grabbers[LeftCenter].contains(point)) {
        ID = LeftCenter;
    } else if (_rect.contains(point)) {
        ID = BoxRegion;
    }

    return ID;
}

void BoxItem::setGrabberCursor(GrabberID id)
{
    QCursor cursor;
    switch (id)
    {
    case BoxRegion:
        cursor = Qt::SizeAllCursor;
        break;
    case TopLeft:
    case BottomRight:
        cursor = Qt::SizeFDiagCursor;
        break;
    case TopRight:
    case BottomLeft:
        cursor = Qt::SizeBDiagCursor;
        break;
    case LeftCenter:
    case RightCenter:
        cursor = Qt::SizeHorCursor;
        break;
    case TopCenter:
    case BottomCenter:
        cursor = Qt::SizeVerCursor;
        break;
    default:
        break;
    }
    QApplication::setOverrideCursor(cursor);
//    setCursor(c);
}

void BoxItem::setGrabbers(qreal width, qreal height)
{
    qreal w = width/2, h = height/2; // int -> qreal, solve grabber transformation bug

    // drawingRegion contains rect and 8 grabbers
    _grabbers[TopLeft     ].setRect(_rect.left()-w,     _rect.top()-h,       width, height);
    _grabbers[TopRight    ].setRect(_rect.right()-w,    _rect.top()-h,       width, height);
    _grabbers[BottomLeft  ].setRect(_rect.left()-w,     _rect.bottom()-h,    width, height);
    _grabbers[BottomRight ].setRect(_rect.right()-w,    _rect.bottom()-h,    width, height);

    _grabbers[LeftCenter  ].setRect(_rect.left()-w,         _rect.center().y()-h,    width, height);
    _grabbers[RightCenter ].setRect(_rect.right()-w,        _rect.center().y()-h,    width, height);
    _grabbers[TopCenter   ].setRect(_rect.center().x()-w,   _rect.top()-h,           width, height);
    _grabbers[BottomCenter].setRect(_rect.center().x()-w,   _rect.bottom()-h,        width, height);

    // cut the grabber size, if intersection exists.
    if (_rect.left() == _sceneRect.left()) {
        _grabbers[TopLeft     ].setLeft(_rect.left());
        _grabbers[TopLeft     ].setWidth(w);
        _grabbers[BottomLeft  ].setLeft(_rect.left());
        _grabbers[BottomLeft  ].setWidth(w);
        _grabbers[LeftCenter  ].setLeft(_rect.left());
        _grabbers[LeftCenter  ].setWidth(w);
    }
    if (_rect.right() == _sceneRect.right()) {
        _grabbers[TopRight     ].setWidth(w);
        _grabbers[BottomRight  ].setWidth(w);
        _grabbers[RightCenter  ].setWidth(w);
    }
    if (_rect.top() == _sceneRect.top()) {
        _grabbers[TopLeft    ].setTop(_rect.top());
        _grabbers[TopLeft    ].setHeight(h);
        _grabbers[TopRight   ].setTop(_rect.top());
        _grabbers[TopRight   ].setHeight(h);
        _grabbers[TopCenter  ].setTop(_rect.top());
        _grabbers[TopCenter  ].setHeight(h);
    }
    if (_rect.bottom() == _sceneRect.bottom()) {
        _grabbers[BottomLeft    ].setHeight(h);
        _grabbers[BottomRight   ].setHeight(h);
        _grabbers[BottomCenter  ].setHeight(h);
    }
}

void BoxItem::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void BoxItem::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void BoxItem::setScale(QRectF fatherRect)
{
    qreal dx = fatherRect.width()/_sceneRect.width();
    qreal dy = fatherRect.height()/_sceneRect.height();

    qreal x = fatherRect.left() + (_rect.left() - _sceneRect.left())*dx;
    qreal y = fatherRect.top() + (_rect.top() - _sceneRect.top())*dy;
    this->_sceneRect = fatherRect;
    this->setRect(x, y, _rect.width()*dx, _rect.height()*dy);
}

void BoxItem::initContextMenu()
{
    _contextMenu.clear();
    foreach (QString name, _typeNameList) {
        _contextMenu.addAction(name);
    }
}

void BoxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QAction *selectedAction = _contextMenu.exec(event->screenPos());
    if (selectedAction) {
        QString name = selectedAction->text();
        if (_typeNameList.contains(name)) {
            emit typeNameChanged(name);
        }
    }
}
