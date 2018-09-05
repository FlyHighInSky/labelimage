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
    _pen(),
    _dragStart(0,0),
    _dragEnd(0,0),
    _sceneRect(sceneRect),
    _grabberWidth(20),
    _grabberHeight(20),
    _drawingRegion(),
    _imageWidth(imageSize.width()),
    _imageHeight(imageSize.height())
{
    _textRect.setDefaultTextColor(QColor(255,255,255,255));
    _textRect.setParentItem(this);
    _textName.setDefaultTextColor(QColor(255,255,255,255));
    _textName.setParentItem(this);
    _pen.setWidth(2);
    _pen.setColor(_color);
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    this->setAcceptHoverEvents(true);
    initContextMenu();
}

void BoxItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    switch (event->buttons()) {
    case Qt::LeftButton:
        _selectedGrabber = getSelectedGrabber(event->pos());
        setGrabberCursor(_selectedGrabber);

        _dragStart = event->pos();
        if (_selectedGrabber != BoxRegion) { // grabber is selected
            _taskStatus = Stretching;
        } else if (_rect.contains(event->pos())) { // box is selected
            _taskStatus = Moving;
        }
        this->setSelected(!this->isSelected());
        break;
    case Qt::RightButton:
        this->setSelected(true);
        break;
    default:
        break;
    }
    _originalRect = _rect;
}

void BoxItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    QRectF rect;
    event->setAccepted(true);
    if (_isMouseMoved) {
        switch (_taskStatus) {
        case Moving:
            _dragEnd = event->pos();
            rect = calculateMoveRect(_dragStart, _dragEnd);
            emit moveCompleted(rect);
            break;
        case Stretching:
//            stretchBox(event->pos());
            this->setCursor(Qt::ArrowCursor);
            _dragEnd = event->pos();
            rect = calculateStretchRect(_dragStart, _dragEnd);
            emit stretchCompleted(rect);
            break;
        default:
            break;
        }
        _taskStatus = Waiting;
        _isMouseMoved = false;
    }
}

void BoxItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    if (this->isSelected()){
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
    if (this->isSelected()){
        this->setCursor(Qt::ArrowCursor);
    }
}

void BoxItem::hoverEnterEvent ( QGraphicsSceneHoverEvent *event )
{
    if (this->isSelected()){
        _selectedGrabber = getSelectedGrabber(event->pos());
        setGrabberCursor(_selectedGrabber);
    }
}

void BoxItem::hoverMoveEvent ( QGraphicsSceneHoverEvent *event )
{
    if (this->isSelected()){
        _selectedGrabber = getSelectedGrabber(event->pos());
        setGrabberCursor(_selectedGrabber);
    }
}

QRectF BoxItem::calculateMoveRect(QPointF dragStart, QPointF dragEnd)
{
    qreal x = dragEnd.x() - dragStart.x() + _originalRect.left();
    qreal y = dragEnd.y() - dragStart.y() + _originalRect.top();

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

    qreal left = _originalRect.left(), top = _originalRect.top();
    qreal right = _originalRect.right(), bottom = _originalRect.bottom();
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

void BoxItem::moveBox(QPointF dragStart, QPointF dragEnd)
{
    qreal x = dragEnd.x() - dragStart.x() + _originalRect.left();
    qreal y = dragEnd.y() - dragStart.y() + _originalRect.top();

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

    this->setRect(x, y, _rect.width(), _rect.height());
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
    setGrabbers();

    qreal halfpw = (_pen.style() == Qt::NoPen) ? qreal(0) : _pen.widthF() / 2;
    if (halfpw > 0.0)
        _boundingRect.adjust(-halfpw, -halfpw, halfpw, halfpw);

    qreal _xScale = _imageWidth*1.0/_sceneRect.width();
    qreal _yScale = _imageHeight*1.0/_sceneRect.height();
    QRect r((int)(_rect.left()*_xScale), (int)(_rect.top()*_yScale),
            (int)(_rect.width()*_xScale), (int)(_rect.height()*_yScale));
    emit boxSelected(r);

    this->update();
}

void BoxItem::setTypeName(QString name)
{
    _typeName = name;
    this->update();
}

QRectF BoxItem::boundingRect() const
{
    return _boundingRect;
}

void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    _pen.setStyle(Qt::SolidLine);
    painter->setPen(_pen);

    // draw the top box, the visible one
    QBrush brush(QColor(255,0,0,0), Qt::SolidPattern);

    painter->setBrush(brush);

    if (option->state & QStyle::State_Selected) {
        for (int i=TopLeft; i<=LeftCenter; i++) {
            painter->fillRect(_grabbers[i], QColor(255, 0, 0, 255));
        }
    }
    painter->drawRect(_rect);

    _textRect.setPos(_rect.topLeft());
    qreal _xScale = _imageWidth*1.0/_sceneRect.width();
    qreal _yScale = _imageHeight*1.0/_sceneRect.height();

    QRect r((int)(_rect.left()*_xScale), (int)(_rect.top()*_yScale),
            (int)(_rect.width()*_xScale), (int)(_rect.height()*_yScale));
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

// 设置鼠标停在拖拽定点处的样式;
void BoxItem::setGrabberCursor(GrabberID id)
{
    switch (id)
    {
    case BoxRegion:
        setCursor(Qt::SizeAllCursor);
        break;
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case LeftCenter:
    case RightCenter:
        setCursor(Qt::SizeHorCursor);
        break;
    case TopCenter:
    case BottomCenter:
        setCursor(Qt::SizeVerCursor);
        break;
    default:
        break;
    }
}

void BoxItem::setGrabbers()
{
    int w = _grabberWidth/2, h = _grabberHeight/2;

    // drawingRegion contains rect and 8 grabbers
    _grabbers[TopLeft     ].setRect(_rect.left()-w,     _rect.top()-h,       _grabberWidth, _grabberHeight);
    _grabbers[TopRight    ].setRect(_rect.right()-w,    _rect.top()-h,       _grabberWidth, _grabberHeight);
    _grabbers[BottomLeft  ].setRect(_rect.left()-w,     _rect.bottom()-h,    _grabberWidth, _grabberHeight);
    _grabbers[BottomRight ].setRect(_rect.right()-w,    _rect.bottom()-h,    _grabberWidth, _grabberHeight);

    _grabbers[LeftCenter  ].setRect(_rect.left()-w,         _rect.center().y()-h,    _grabberWidth, _grabberHeight);
    _grabbers[RightCenter ].setRect(_rect.right()-w,        _rect.center().y()-h,    _grabberWidth, _grabberHeight);
    _grabbers[TopCenter   ].setRect(_rect.center().x()-w,   _rect.top()-h,           _grabberWidth, _grabberHeight);
    _grabbers[BottomCenter].setRect(_rect.center().x()-w,   _rect.bottom()-h,        _grabberWidth, _grabberHeight);

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
//            setTypeName(name);
            emit targetTypeChanged(name);
        }
    }
}
