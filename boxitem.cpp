#include "boxitem.h"
#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

BoxItem::BoxItem(QRectF fatherRect, QSize imageSize, QList<QString> *classNameList, QString labelClassName):
    _classNameList(classNameList),
    _textRect(),
    _textName(),
    _labelClassName(labelClassName),
    _color(Qt::red),
    _pen(),
    _dragStart(0,0),
    _fatherRect(fatherRect),
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

void BoxItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    switch (_taskStatus) {
    case Moving:
        moveBox(event->pos());
        break;
    case Stretching:
        stretchBox(event->pos());
        this->setCursor(Qt::ArrowCursor);
        break;
    default:
        break;
    }
    _taskStatus = Waiting;
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
        } else if (_box.contains(event->pos())) { // box is selected
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
    _oldBox = _box;
}

void BoxItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    if (this->isSelected()){
        switch (_taskStatus) {
        case Moving:
            moveBox(event->pos());
            break;
        case Stretching:
            stretchBox(event->pos());
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

void BoxItem::moveBox(QPointF pos)
{
    qreal x = pos.x() - _dragStart.x() + _oldBox.left();
    qreal y = pos.y() - _dragStart.y() + _oldBox.top();

    if (x <= _fatherRect.left()) {
        x = _fatherRect.left();
    }
    if (y <= _fatherRect.top()) {
        y = _fatherRect.top();
    }
    if (_fatherRect.right()-x <= _box.width()) {
        x = _fatherRect.right() - _box.width();
    }

    if (_fatherRect.bottom()-y <= _box.height()) {
        y = _fatherRect.bottom() - _box.height();
    }

    this->setRect(x, y, _box.width(), _box.height());
}

void BoxItem::stretchBox(QPointF pos)
{
    qreal dx = pos.x() - _dragStart.x();
    qreal dy = pos.y() - _dragStart.y();

    qreal left = _oldBox.left(), top = _oldBox.top();
    qreal right = _oldBox.right(), bottom = _oldBox.bottom();
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
    this->setRect(newLeft, newTop, newRight-newLeft, newBottom-newTop);
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

    _box = rect;
    if (_fatherRect.intersects(_boundingRect)) {
        if (!_fatherRect.contains(_boundingRect)) {
            _boundingRect = _fatherRect.intersected(_boundingRect);
            _box = rect.intersected(_boundingRect);
        }
    }
    setGrabbers();

    qreal halfpw = (_pen.style() == Qt::NoPen) ? qreal(0) : _pen.widthF() / 2;
    if (halfpw > 0.0)
        _boundingRect.adjust(-halfpw, -halfpw, halfpw, halfpw);

    qreal _xScale = _imageWidth*1.0/_fatherRect.width();
    qreal _yScale = _imageHeight*1.0/_fatherRect.height();
    QRect r((int)(_box.left()*_xScale), (int)(_box.top()*_yScale),
            (int)(_box.width()*_xScale), (int)(_box.height()*_yScale));
    emit boxSelected(r);

    this->update();
}
void BoxItem::setLabelClassName(QString name)
{
    _labelClassName = name;
}

void BoxItem::setLabelRect(QRectF rect)
{
    _labelRect = rect;
}

void BoxItem::setLabelRect(qreal x, qreal y, qreal w, qreal h)
{
    setLabelRect(QRectF(x,y,w,h));
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
    painter->drawRect(_box);

    _textRect.setPos(_box.topLeft());
    qreal _xScale = _imageWidth*1.0/_fatherRect.width();
    qreal _yScale = _imageHeight*1.0/_fatherRect.height();

    QRect r((int)(_box.left()*_xScale), (int)(_box.top()*_yScale),
            (int)(_box.width()*_xScale), (int)(_box.height()*_yScale));
    QString rectInfo = QString("[%1,%2,%3,%4]")
            .arg(r.left()).arg(r.top())
            .arg(r.width()).arg(r.height());
//    emit boxSelected(r);

    _textRect.setPlainText(rectInfo);

    _textName.setPos(_box.bottomLeft());
    _textName.setPlainText(_labelClassName);
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
    } else if (_box.contains(point)) {
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
    _grabbers[TopLeft     ].setRect(_box.left()-w,     _box.top()-h,       _grabberWidth, _grabberHeight);
    _grabbers[TopRight    ].setRect(_box.right()-w,    _box.top()-h,       _grabberWidth, _grabberHeight);
    _grabbers[BottomLeft  ].setRect(_box.left()-w,     _box.bottom()-h,    _grabberWidth, _grabberHeight);
    _grabbers[BottomRight ].setRect(_box.right()-w,    _box.bottom()-h,    _grabberWidth, _grabberHeight);

    _grabbers[LeftCenter  ].setRect(_box.left()-w,         _box.center().y()-h,    _grabberWidth, _grabberHeight);
    _grabbers[RightCenter ].setRect(_box.right()-w,        _box.center().y()-h,    _grabberWidth, _grabberHeight);
    _grabbers[TopCenter   ].setRect(_box.center().x()-w,   _box.top()-h,           _grabberWidth, _grabberHeight);
    _grabbers[BottomCenter].setRect(_box.center().x()-w,   _box.bottom()-h,        _grabberWidth, _grabberHeight);

    // cut the grabber size, if intersection exists.
    if (_box.left() == _fatherRect.left()) {
        _grabbers[TopLeft     ].setLeft(_box.left());
        _grabbers[TopLeft     ].setWidth(w);
        _grabbers[BottomLeft  ].setLeft(_box.left());
        _grabbers[BottomLeft  ].setWidth(w);
        _grabbers[LeftCenter  ].setLeft(_box.left());
        _grabbers[LeftCenter  ].setWidth(w);
    }
    if (_box.right() == _fatherRect.right()) {
        _grabbers[TopRight     ].setWidth(w);
        _grabbers[BottomRight  ].setWidth(w);
        _grabbers[RightCenter  ].setWidth(w);
    }
    if (_box.top() == _fatherRect.top()) {
        _grabbers[TopLeft    ].setTop(_box.top());
        _grabbers[TopLeft    ].setHeight(h);
        _grabbers[TopRight   ].setTop(_box.top());
        _grabbers[TopRight   ].setHeight(h);
        _grabbers[TopCenter  ].setTop(_box.top());
        _grabbers[TopCenter  ].setHeight(h);
    }
    if (_box.bottom() == _fatherRect.bottom()) {
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
    qreal dx = fatherRect.width()/_fatherRect.width();
    qreal dy = fatherRect.height()/_fatherRect.height();

    qreal x = fatherRect.left() + (_box.left() - _fatherRect.left())*dx;
    qreal y = fatherRect.top() + (_box.top() - _fatherRect.top())*dy;
    this->_fatherRect = fatherRect;
    this->setRect(x, y, _box.width()*dx, _box.height()*dy);
}
void BoxItem::initContextMenu()
{
    _contextMenu.clear();
    foreach (QString name, *_classNameList) {
        _contextMenu.addAction(name);
    }
}
void BoxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QAction *selectedAction = _contextMenu.exec(event->screenPos());
    if (selectedAction) {
        QString name = selectedAction->text();
        if (_classNameList->contains(name)) {
            setLabelClassName(name);
        }
    }
}
