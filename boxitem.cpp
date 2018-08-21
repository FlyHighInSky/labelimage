//#include "boxitem.h"

//#include <QBrush>
//#include <QLinearGradient>
//#include <QDebug>

//#include "math.h"

///**
//  *  This box can be re-sized and it can be moved. For moving, we capture
//  *  the mouse move events and move the box location accordingly.
//  *
//  *  To resize the box, we place small indicator boxes on the four corners that give the user
//  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
//  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
//  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
//  *  but the StateBox object (which owns the corners) captures and processes the mouse
//  *  events on behalf of the CornerGrabbers (because the owner wants to be
//  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
//  *  on the CornerGrabber objects:
//          _corners[0]->installSceneEventFilter(this);
//  *
//  *
//  *
//  */

//BoxItem::BoxItem(QRectF fatherRect):
//    _text(),
//    _color(Qt::black),
//    _pen(),
//    _location(0,0),
//    _dragStart(0,0),
////    _box(QRectF(100,100,100,100)),
//    _fatherRect(fatherRect),
//    _cornerDragStart(0,0),
//    _grabberWidth(20),
//    _grabberHeight(20),
//    _drawingRegion()
//{
//    _pen.setWidth(2);
//    _pen.setColor(_color);
//    //    _text.setPos(35,35);
//    //    _text.setPlainText("text goes here");
//    //    _text.setParentItem(this);
//    setRect(_box);
//    this->setAcceptHoverEvents(true);
//}


///**
//  * This scene event filter has been registered with all four corner grabber items.
//  * When called, a pointer to the sending item is provided along with a generic
//  * event.  A dynamic_cast is used to determine if the event type is one of the events
//  * we are interrested in.
//  */
//bool BoxItem::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
//{
//    qDebug() << " QEvent == " + QString::number(event->type());

//    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
//    if ( corner == NULL)
//        return false; // not expected to get here

//    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
//    if ( mevent == NULL) {
//        // this is not one of the mouse events we are interrested in
//        return false;
//    }

//    switch ( event->type() ) // if the mouse went down, record the x,y coords of the press, record it inside the corner object
//    {
//    case QEvent::GraphicsSceneMousePress: {
//        corner->setMouseState(CornerGrabber::kMouseDown);
//        corner->mouseDownX = mevent->pos().x();
//        corner->mouseDownY = mevent->pos().y();
//    }
//        break;
//    case QEvent::GraphicsSceneMouseRelease: {
//        corner->setMouseState(CornerGrabber::kMouseReleased);
//    }
//        break;
//    case QEvent::GraphicsSceneMouseMove: {
//        corner->setMouseState(CornerGrabber::kMouseMoving );
//    }
//        break;
//    default: // we dont care about the rest of the events
//        return false;
//    }

//    if ( corner->getMouseState() == CornerGrabber::kMouseMoving ) {
//        int dx = mevent->pos().x() - corner->mouseDownX;
//        int dy = mevent->pos().y() - corner->mouseDownY;

//        // depending on which corner has been grabbed, we want to move the position
//        // of the item as it grows/shrinks accordingly. so we need to eitehr add
//        // or subtract the offsets based on which corner this is.
//        qreal left = _box.left(), top = _box.top();
//        qreal right = _box.right(), bottom = _box.bottom();
//        switch( corner->getCorner() )
//        {
//        case TopLeft:
//        {
//            left = qMin(left+dx, right);
//            right = qMax(left+dx, right);
//            top = qMin(top+dy, bottom);
//            bottom = qMax(top+dy, bottom);
//        }
//            break;
//        case TopCenter:
//        {
//            top = qMin(top+dy, bottom);
//            bottom = qMax(top+dy, bottom);
//        }
//            break;
//        case TopRight:
//        {
//            left = qMin(left, right+dx);
//            right = qMax(left, right+dx);
//            top = qMin(top+dy, bottom);
//            bottom = qMax(top+dy, bottom);
//        }
//            break;
//        case RightCenter:
//        {
//            left = qMin(left, right+dx);
//            right = qMax(left, right+dx);
//        }
//            break;
//        case BottomRight:
//        {
//            left = qMin(left, right+dx);
//            right = qMax(left, right+dx);
//            top = qMin(top, bottom+dy);
//            bottom = qMax(top, bottom+dy);
//        }
//            break;
//        case BottomCenter:
//        {
//            top = qMin(top, bottom+dy);
//            bottom = qMax(top, bottom+dy);
//        }
//            break;
//        case BottomLeft:
//        {
//            left = qMin(left+dx, right);
//            right = qMax(left+dx, right);
//            top = qMin(top, bottom+dy);
//            bottom = qMax(top, bottom+dy);
//        }
//            break;
//        case LeftCenter:
//        {
//            left = qMin(left+dx, right);
//            right = qMax(left+dx, right);
//        }
//            break;
//        }
//        this->setRect(QRectF(left, top, right-left, bottom-top));
//    }

//    setCornerPositions();
//    this->update();
//    return true;// true => do not send event to watched - we are finished with this event
//}



//// for supporting moving the box across the scene
//void BoxItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
//{
//    event->setAccepted(true);
//    _location.setX( static_cast<int>(_location.x()));
//    _location.setY( static_cast<int>(_location.y()));
//    this->setPos(_location);
//}

//// for supporting moving the box across the scene
//void BoxItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
//{
//    event->setAccepted(true);
//    _dragStart = event->pos();
//    setSelected(true);
//}


//// for supporting moving the box across the scene
//void BoxItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
//{
//    QPointF newPos = event->pos() ;
//    _location += (newPos - _dragStart);
//    this->setPos(_location);
//}

//// remove the corner grabbers
//void BoxItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
//{
//    _color = Qt::black;
//    for (int i=TopLeft; i<=LeftCenter; i++) {
//        _corners[i]->setParentItem(NULL);
//        delete _corners[i];
//    }
//}

//// create the corner grabbers
//void BoxItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
//{
//    _color = Qt::red;
//    for (int i=TopLeft; i<=LeftCenter; i++) {
//        _corners[i] = new CornerGrabber(this,i,_grabberWidth,_grabberHeight);
//        _corners[i]->installSceneEventFilter(this);
//    }
//    setCornerPositions();
//}

//void BoxItem::setRect(const QRectF &rect)
//{
//    prepareGeometryChange();
//    _boundingRect.setRect(rect.left()-_grabberWidth/2, rect.top()-_grabberHeight/2,
//                           rect.width()+_grabberWidth, rect.height()+_grabberHeight);

//    _box = rect;
//    if (_fatherRect.intersects(_boundingRect)) {
//        if (!_fatherRect.contains(_boundingRect)) {
//            _boundingRect = _fatherRect.intersected(_boundingRect);
//            _box = rect.intersected(_boundingRect);
//        }
//    }
//    setGrabbers();

//    qreal halfpw = (_pen.style() == Qt::NoPen) ? qreal(0) : _pen.widthF() / 2;
//    if (halfpw > 0.0)
//        _boundingRect.adjust(-halfpw, -halfpw, halfpw, halfpw);

//    this->update();
//}

//void BoxItem::setCornerPositions()
//{
//    int w = _grabberWidth/2, h = _grabberHeight/2;
//    //    _corners[0]->setPos(_drawingOrigenX-w, _drawingOrigenY-h);
//    //    _corners[1]->setPos((_drawingOrigenX+_drawingWidth)/2, _drawingOrigenY-h);
//    //    _corners[2]->setPos(_drawingWidth-w,  _drawingOrigenY-h);

//    //    _corners[3]->setPos(_drawingWidth-w, (_drawingHeight+_drawingOrigenY)/2);
//    //    _corners[4]->setPos(_drawingWidth-w, _drawingHeight-h);

//    //    _corners[5]->setPos((_drawingOrigenX+_drawingWidth)/2, _drawingHeight-h);
//    //    _corners[6]->setPos(_drawingOrigenX-w, _drawingHeight-h);
//    //    _corners[7]->setPos(_drawingOrigenX-w, (_drawingHeight+_drawingOrigenY)/2);

//    // drawingRegion contains rect and 8 grabbers
//    _corners[TopLeft     ]->setRect(_box.left()-w,     _box.top()-h,       _grabberWidth, _grabberHeight);
//    _corners[TopRight    ]->setRect(_box.right()-w,    _box.top()-h,       _grabberWidth, _grabberHeight);
//    _corners[BottomLeft  ]->setRect(_box.left()-w,     _box.bottom()-h,    _grabberWidth, _grabberHeight);
//    _corners[BottomRight ]->setRect(_box.right()-w,    _box.bottom()-h,    _grabberWidth, _grabberHeight);

//    _corners[LeftCenter  ]->setRect(_box.left()-w,         _box.center().y()-h,    _grabberWidth, _grabberHeight);
//    _corners[RightCenter ]->setRect(_box.right()-w,        _box.center().y()-h,    _grabberWidth, _grabberHeight);
//    _corners[TopCenter   ]->setRect(_box.center().x()-w,   _box.top()-h,           _grabberWidth, _grabberHeight);
//    _corners[BottomCenter]->setRect(_box.center().x()-w,   _box.bottom()-h,        _grabberWidth, _grabberHeight);

//    // cut the grabber size, if intersection exists.
//    if (_box.left() == _fatherRect.left()) {
//        _corners[TopLeft     ]->setLeft(_box.left());
//        _corners[TopLeft     ]->setWidth(w);
//        _corners[BottomLeft  ]->setLeft(_box.left());
//        _corners[BottomLeft  ]->setWidth(w);
//        _corners[LeftCenter  ]->setLeft(_box.left());
//        _corners[LeftCenter  ]->setWidth(w);
//    }
//    if (_box.right() == _fatherRect.right()) {
//        _corners[TopRight     ]->setWidth(w);
//        _corners[BottomRight  ]->setWidth(w);
//        _corners[RightCenter  ]->setWidth(w);
//    }
//    if (_box.top() == _fatherRect.top()) {
//        _corners[TopLeft    ]->setTop(_box.top());
//        _corners[TopLeft    ]->setHeight(h);
//        _corners[TopRight   ]->setTop(_box.top());
//        _corners[TopRight   ]->setHeight(h);
//        _corners[TopCenter  ]->setTop(_box.top());
//        _corners[TopCenter  ]->setHeight(h);
//    }
//    if (_box.bottom() == _fatherRect.bottom()) {
//        _corners[BottomLeft    ]->setHeight(h);
//        _corners[BottomRight   ]->setHeight(h);
//        _corners[BottomCenter  ]->setHeight(h);
//    }
//}


//void BoxItem::setGrabbers()
//{
//    int w = _grabberWidth/2, h = _grabberHeight/2;

//    // drawingRegion contains rect and 8 grabbers
//    _grabbers[TopLeft     ].setRect(_box.left()-w,     _box.top()-h,       _grabberWidth, _grabberHeight);
//    _grabbers[TopRight    ].setRect(_box.right()-w,    _box.top()-h,       _grabberWidth, _grabberHeight);
//    _grabbers[BottomLeft  ].setRect(_box.left()-w,     _box.bottom()-h,    _grabberWidth, _grabberHeight);
//    _grabbers[BottomRight ].setRect(_box.right()-w,    _box.bottom()-h,    _grabberWidth, _grabberHeight);

//    _grabbers[LeftCenter  ].setRect(_box.left()-w,         _box.center().y()-h,    _grabberWidth, _grabberHeight);
//    _grabbers[RightCenter ].setRect(_box.right()-w,        _box.center().y()-h,    _grabberWidth, _grabberHeight);
//    _grabbers[TopCenter   ].setRect(_box.center().x()-w,   _box.top()-h,           _grabberWidth, _grabberHeight);
//    _grabbers[BottomCenter].setRect(_box.center().x()-w,   _box.bottom()-h,        _grabberWidth, _grabberHeight);

//    // cut the grabber size, if intersection exists.
//    if (_box.left() == _fatherRect.left()) {
//        _grabbers[TopLeft     ].setLeft(_box.left());
//        _grabbers[TopLeft     ].setWidth(w);
//        _grabbers[BottomLeft  ].setLeft(_box.left());
//        _grabbers[BottomLeft  ].setWidth(w);
//        _grabbers[LeftCenter  ].setLeft(_box.left());
//        _grabbers[LeftCenter  ].setWidth(w);
//    }
//    if (_box.right() == _fatherRect.right()) {
//        _grabbers[TopRight     ].setWidth(w);
//        _grabbers[BottomRight  ].setWidth(w);
//        _grabbers[RightCenter  ].setWidth(w);
//    }
//    if (_box.top() == _fatherRect.top()) {
//        _grabbers[TopLeft    ].setTop(_box.top());
//        _grabbers[TopLeft    ].setHeight(h);
//        _grabbers[TopRight   ].setTop(_box.top());
//        _grabbers[TopRight   ].setHeight(h);
//        _grabbers[TopCenter  ].setTop(_box.top());
//        _grabbers[TopCenter  ].setHeight(h);
//    }
//    if (_box.bottom() == _fatherRect.bottom()) {
//        _grabbers[BottomLeft    ].setHeight(h);
//        _grabbers[BottomRight   ].setHeight(h);
//        _grabbers[BottomCenter  ].setHeight(h);
//    }
//}

//QRectF BoxItem::boundingRect() const
//{
////    return QRectF(0, 0, _width+_grabberWidth, _height+_grabberHeight);
////    if (_boundingRect.isNull()) {
//        qreal halfpw = (_pen.style() == Qt::NoPen) ? qreal(0) : _pen.widthF() / 2;
////        _boundingRect = _drawingRegion;
//        if (halfpw > 0.0)
//            ;
////            _boundingRect.adjust(-halfpw, -halfpw, halfpw, halfpw);
////    }

//    return _boundingRect;
//}

//// example of a drop shadow effect on a box, using QLinearGradient and two boxes
//void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
//{
//    /*
//     The drop shadow effect will be created by drawing a filled, rounded corner rectangle with a gradient fill.
//     Then on top of this will be drawn  filled, rounded corner rectangle, filled with a solid color, and offset such that the gradient filled
//     box is only visible below for a few pixels on two edges.

//     The total box size is _width by _height. So the top box will start at (0,0) and go to (_width-shadowThickness, _height-shadowThickness),
//     while the under box will be offset, and start at (shadowThickness+0, shadowThickness+0) and go to  (_width, _height).
//       */

//    _pen.setStyle(Qt::SolidLine);
//    painter->setPen(_pen);

//    // draw the top box, the visible one
//    QBrush brush(QColor(0,0,0,0), Qt::SolidPattern);

//    painter->setBrush( brush);

//    //    QPointF topLeft (_drawingOrigenX, _drawingOrigenY);
//    //    QPointF bottomRight ( _drawingWidth, _drawingHeight);

//    //    QRectF rect(topLeft, bottomRight);
//    painter->drawRect(_box);
//    painter->drawRects(_grabbers, 8);
////    for (int i=LeftTop; i<=LeftCenter; i++) {
////        painter->drawRect(_grabbers[i]);
////    }
//}


//void BoxItem::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
//{
//    event->setAccepted(false);
//}

//void BoxItem::mousePressEvent(QGraphicsSceneDragDropEvent *event)
//{
//    event->setAccepted(false);
//}



#include "boxitem.h"

#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

#include "math.h"

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the StateBox object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  *
  *
  *
  */

BoxItem::BoxItem(QRectF fatherRect):
    _text(),
    _color(Qt::black),
    _pen(),
//    _oldBox(0,00),
    _dragStart(0,0),
    _fatherRect(fatherRect),
    _cornerDragStart(0,0),
    _grabberWidth(20),
    _grabberHeight(20),
    _drawingRegion()
{
    _pen.setWidth(2);
    _pen.setColor(_color);
    //    _text.setPos(35,35);
    //    _text.setPlainText("text goes here");
    //    _text.setParentItem(this);
//    setRect(_box);
    this->setAcceptHoverEvents(true);
}


/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */
bool BoxItem::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
//    switch ( event->type() ) // if the mouse went down, record the x,y coords of the press, record it inside the corner object
//    {
//    case QEvent::GraphicsSceneMousePress: {
////            _stretchRectState = getStrethRectState(event->Scence());
////            if (_stretchRectState != None) {
//////                m_currentCaptureState = BeginMoveStretchRect;
////                setStretchCursorStyle(_stretchRectState);// 当前鼠标在拖动选中区顶点时,设置鼠标当前状态;
//////                m_beginMovePoint = event->pos();
////            }
//////            else if (isPressPointInSelectRect(event->pos()))// 是否在选中的矩形中;
//////            {
//////                m_currentCaptureState = BeginMoveCaptureArea;
//////                m_beginMovePoint = event->pos();
//////            }
//        }
//        break;
//    case QEvent::GraphicsSceneMouseRelease: {
//        corner->setMouseState(CornerGrabber::kMouseReleased);
//    }
//        break;
//    case QEvent::GraphicsSceneMouseMove: {
//        corner->setMouseState(CornerGrabber::kMouseMoving );
//    }
//        break;
//    default: // we dont care about the rest of the events
//        return false;
//    }

//    if ( corner->getMouseState() == CornerGrabber::kMouseMoving ) {
//        int dx = mevent->pos().x() - corner->mouseDownX;
//        int dy = mevent->pos().y() - corner->mouseDownY;

//        // depending on which corner has been grabbed, we want to move the position
//        // of the item as it grows/shrinks accordingly. so we need to eitehr add
//        // or subtract the offsets based on which corner this is.
//        qreal left = _box.left(), top = _box.top();
//        qreal right = _box.right(), bottom = _box.bottom();
//        switch( corner->getCorner() )
//        {
//        case TopLeft:
//        {
//            left = qMin(left+dx, right);
//            right = qMax(left+dx, right);
//            top = qMin(top+dy, bottom);
//            bottom = qMax(top+dy, bottom);
//        }
//            break;
//        case TopCenter:
//        {
//            top = qMin(top+dy, bottom);
//            bottom = qMax(top+dy, bottom);
//        }
//            break;
//        case TopRight:
//        {
//            left = qMin(left, right+dx);
//            right = qMax(left, right+dx);
//            top = qMin(top+dy, bottom);
//            bottom = qMax(top+dy, bottom);
//        }
//            break;
//        case RightCenter:
//        {
//            left = qMin(left, right+dx);
//            right = qMax(left, right+dx);
//        }
//            break;
//        case BottomRight:
//        {
//            left = qMin(left, right+dx);
//            right = qMax(left, right+dx);
//            top = qMin(top, bottom+dy);
//            bottom = qMax(top, bottom+dy);
//        }
//            break;
//        case BottomCenter:
//        {
//            top = qMin(top, bottom+dy);
//            bottom = qMax(top, bottom+dy);
//        }
//            break;
//        case BottomLeft:
//        {
//            left = qMin(left+dx, right);
//            right = qMax(left+dx, right);
//            top = qMin(top, bottom+dy);
//            bottom = qMax(top, bottom+dy);
//        }
//            break;
//        case LeftCenter:
//        {
//            left = qMin(left+dx, right);
//            right = qMax(left+dx, right);
//        }
//            break;
//        }
//        this->setRect(QRectF(left, top, right-left, bottom-top));
//    }

//    setCornerPositions();
//    this->update();
    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
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

// for supporting moving the box across the scene
void BoxItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    if (event->button() == Qt::LeftButton) {
        _selectedGrabber = getSelectedGrabber(event->pos());
        setGrabberCursor(_selectedGrabber);

        _dragStart = event->pos();
        if (_selectedGrabber != None) { // grabber is selected
            _taskStatus = Stretching;
        } else if (_box.contains(event->pos())) { // box is selected
            _taskStatus = Moving;
        }
    }
    _oldBox = _box;
    setSelected(true);
}

// for supporting moving the box across the scene
void BoxItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    switch (_taskStatus) {
    case Moving:
        moveBox(event->pos());
        break;
    case Stretching:
        stretchBox(event->pos());
        break;
    default:
        break;
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

    this->update();
}

QRectF BoxItem::boundingRect() const
{
    return _boundingRect;
}

// example of a drop shadow effect on a box, using QLinearGradient and two boxes
void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    /*
     The drop shadow effect will be created by drawing a filled, rounded corner rectangle with a gradient fill.
     Then on top of this will be drawn  filled, rounded corner rectangle, filled with a solid color, and offset such that the gradient filled
     box is only visible below for a few pixels on two edges.

     The total box size is _width by _height. So the top box will start at (0,0) and go to (_width-shadowThickness, _height-shadowThickness),
     while the under box will be offset, and start at (shadowThickness+0, shadowThickness+0) and go to  (_width, _height).
       */

    _pen.setStyle(Qt::SolidLine);
    painter->setPen(_pen);

    // draw the top box, the visible one
    QBrush brush(QColor(0,0,0,0), Qt::SolidPattern);

    painter->setBrush( brush);

    //    QPointF topLeft (_drawingOrigenX, _drawingOrigenY);
    //    QPointF bottomRight ( _drawingWidth, _drawingHeight);

    //    QRectF rect(topLeft, bottomRight);
    painter->drawRect(_box);
    painter->drawRects(_grabbers, 8);
//    for (int i=LeftTop; i<=LeftCenter; i++) {
//        painter->drawRect(_grabbers[i]);
//    }
}

GrabberID BoxItem::getSelectedGrabber(QPointF point)
{
    GrabberID ID = None;
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
        ID = None;
    }

    return ID;
}

// 设置鼠标停在拖拽定点处的样式;
void BoxItem::setGrabberCursor(GrabberID stretchRectState)
{
    switch (stretchRectState)
    {
    case None:
        setCursor(Qt::ArrowCursor);
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

// 获取拖拽后的矩形选中区域;
QRectF BoxItem::getStretchRect()
{
    QRectF stretchRect;
//    QRectF currentRect = getRect(m_beginPoint, m_endPoint);
//    switch (m_stretchRectState)
//    {
//    case None:
//        stretchRect = getRect(m_beginPoint, m_endPoint);
//        break;
//    case TopLeft:
//    {
//        stretchRect = getRect(currentRect.bottomRight(), m_endMovePoint);
//    }
//        break;
//    case TopRight:
//    {
//        QPoint beginPoint = QPoint(currentRect.topLeft().x(), m_endMovePoint.y());
//        QPoint endPoint = QPoint(m_endMovePoint.x(), currentRect.bottomRight().y());
//        stretchRect = getRect(beginPoint, endPoint);
//    }
//        break;
//    case BottomLeft:
//    {
//        QPoint beginPoint = QPoint(m_endMovePoint.x() , currentRect.topLeft().y());
//        QPoint endPoint = QPoint(currentRect.bottomRight().x(), m_endMovePoint.y());
//        stretchRect = getRect(beginPoint, endPoint);
//    }
//        break;
//    case BottomRight:
//    {
//        stretchRect = getRect(currentRect.topLeft(), m_endMovePoint);
//    }
//        break;
//    case LeftCenter:
//    {
//        QPoint beginPoint = QPoint(m_endMovePoint.x(), currentRect.topLeft().y());
//        stretchRect = getRect(beginPoint, currentRect.bottomRight());
//    }
//        break;
//    case TopCenter:
//    {
//        QPoint beginPoint = QPoint(currentRect.topLeft().x(), m_endMovePoint.y());
//        stretchRect = getRect(beginPoint, currentRect.bottomRight());
//    }
//        break;
//    case RightCenter:
//    {
//        QPoint endPoint = QPoint(m_endMovePoint.x(), currentRect.bottomRight().y());
//        stretchRect = getRect(currentRect.topLeft(), endPoint);
//    }
//        break;
//    case BottomCenter:
//    {
//        QPoint endPoint = QPoint(currentRect.bottomRight().x(), m_endMovePoint.y());
//        stretchRect = getRect(currentRect.topLeft(), endPoint);
//    }
//        break;
//    default:
//    {
//        stretchRect = getRect(m_beginPoint , m_endPoint );
//    }
//        break;
//    }

//    // 拖动结束更新 m_beginPoint , m_endPoint;
//    if (m_currentCaptureState == FinishMoveStretchRect)
//    {
//        m_beginPoint = stretchRect.topLeft();
//        m_endPoint = stretchRect.bottomRight();
//    }

    return stretchRect;
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
