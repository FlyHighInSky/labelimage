
/*
  * Copyright 2010, David W. Drell
  *
  * This is free sample code. Use as you wish, no warranties.
  */

#include "boxitem.h"
#include <QDebug>
#include <QBrush>
#include <QLinearGradient>

#include "math.h"

BoxItem::BoxItem():
        _text(),
        _outterborderColor(Qt::black),
        _outterborderPen(),
        _location(0,0),
        _dragStart(0,0),
        _gridSpace(10),
        _width(150),
        _height(100)
{
    _outterborderPen.setWidth(2);
    _outterborderPen.setColor(_outterborderColor);

    _text.setPlainText("text goes here");
    _text.setParentItem(this);

    this->setAcceptHoverEvents(true);
}

/* re-implement the mouse event handlers from the base QGraphicsItem class
  there are two versions provided in the base class, two that
  take QGraphicsSceneDragDropEvent - which I will ignore - and two
  that take QGraphicsSceneMouseEvent - which I will use
  */

void BoxItem::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void BoxItem::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void BoxItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);// tell the base class we are handling this event

    // force my box to snap to grid, just truncate the pixel number and
    // snap to the next lowest grid value
    _location.setX( ( static_cast<int>(_location.x()) / _gridSpace) * _gridSpace );
    _location.setY( ( static_cast<int>(_location.y()) / _gridSpace) * _gridSpace );
    this->setPos(_location);
}

void BoxItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    // allow the user to drag the box, capture the starting position on mouse-down
    event->setAccepted(true);
    _dragStart = event->pos();
}

void BoxItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    // user have moved the mouse, move the location of the box
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);
    this->setPos(_location);
}

void BoxItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    // return the box color to black when the mouse is no longer hovering
    _outterborderColor = Qt::black;
    this->update(0,0,_width,_height);
}

void BoxItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    // draw the box in red if the mouse is hovering over it
    _outterborderColor = Qt::red;
    this->update(0,0,_width,_height);
}

// boundingRect must be re-implemented from the base class to provide the scene with
// size info about this custom GraphicsItem

QRectF BoxItem::boundingRect() const
{
    return QRectF(0,0,_width,_height);
}

void BoxItem::setRect(QRect r)
{
    rect = r;
    update();
}

void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // draw the outter box
    _outterborderPen.setColor( _outterborderColor );
    _outterborderPen.setStyle(Qt::SolidLine);

    painter->setPen(_outterborderPen);

    QBrush brush(QColor::fromRgb(255,255,255,255), Qt::SolidPattern);
    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setBrush(brush);

    painter->drawRect(rect);
}
