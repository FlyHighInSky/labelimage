/*
  * Copyright 2010, David W. Drell
  *
  * This is free sample code. Use as you wish, no warranties.
  */

#ifndef STATEBOX_H
#define STATEBOX_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QCursor>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include "cornergrabber.h"



enum GrabberID{
    TopLeft = 0,
    TopCenter,
    TopRight,
    RightCenter,
    BottomRight,
    BottomCenter,
    BottomLeft,
    LeftCenter,
    BoxRegion
};

enum TaskStatus {
    Moving = 0,
    Stretching,
    Waiting
};

/**
  * \class StateBox
  * This is short example/demonstration of creating a custom Qt QGraphicsItem.
  *  Example usage of a custom QGraphicsItem in a QGraphicsScene with examples of mouse hover events.
  *
  *  My goal was to create a simpe box, in the shape of a UML State/Class box, with a title
  *  area seprated from a main arear below by line.
  *
  *  This sample class inherits from QGraphicsItem, and must reimplement boundingRect() and paint()
  *  from the base class.
  *
  *  To learn about handling mouse events in a QGraphicsScene, I wanted my box to turn red when the mouse moves inside, and black
  *  when the mouse moves out.
  */

class BoxItem : public QGraphicsItem
{
public:
    BoxItem(QRectF fatherRect);
    QGraphicsTextItem _text;    ///< sample text to go in the title area.

    void setScale(QRectF fatherRect, qreal factor);
    void setRect(const QRectF &rect);
    void setRect(const qreal x, qreal y, qreal w, qreal h);
    enum { Type = UserType + 1 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

private:

    virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent *event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent *event ); ///< must be re-implemented to handle mouse hover leave events
    virtual void hoverMoveEvent (QGraphicsSceneHoverEvent * event);

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );///< allows the main object to be moved in the scene by capturing the mouse move events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) ;

    void setCornerPositions();
    void setGrabbers();
    void moveBox(QPointF pos);
    void stretchBox(QPointF pos);

    GrabberID getSelectedGrabber(QPointF point);
    QRectF getStretchRect();
    void setGrabberCursor(GrabberID stretchRectState);
    GrabberID _selectedGrabber;
    TaskStatus _taskStatus = Waiting;

    QRectF _fatherRect;
    QRectF _box;
    QRectF _drawingRegion;
    QRectF _boundingRect;

    QColor _color; ///< the hover event handlers will toggle this between red and black
    QPen _pen; ///< the pen is used to paint the red/black border
    QRectF _oldBox;
    QPointF _dragStart, _dragEnd;

    int _grabberWidth;
    int _grabberHeight;

    QRectF _grabbers[8];

};

#endif // STATEBOX_H
