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
#include <QMenu>
#include <QApplication>

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

class BoxItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    BoxItem(QRectF sceneRect, QSize imageSize, QStringList &targetTypeNameList, QString targetTypeName);
    ~BoxItem()
    {
       ;
    }

    void setTypeName(QString name);
    QString typeName() const
    {
        return _typeName;
    }
    void setScale(QRectF fatherRect);
    void setRect(const QRectF &rect);
    void setRect(const qreal x, const qreal y, const qreal w, const qreal h);
    void rect(qreal *info) const
    {
        qreal ws = 1.0 / _sceneRect.width();
        qreal hs = 1.0 / _sceneRect.height();

        info[0] = _rect.center().x()*ws;
        info[1] = _rect.center().y()*hs;
        info[2] = _rect.width()*ws;
        info[3] = _rect.height()*hs;
    }
    QRectF rect() const
    {
        return _rect;
    }
    QRectF _oldRect;

    QCursor oldCursor() const
    {
        return _oldCursor;
    }
    void setOldCursor(QCursor &c)
    {
        _oldCursor = c;
    }
    enum { Type = UserType + 1 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

signals:
    void boxSelected(QRect boxRect, QString typeName);
    void typeNameChanged(QString newTypeName);
    void stretchCompleted(QRectF newRect, QRectF oleRect);
    void moveCompleted(QRectF newRect, QRectF oleRect);

private:

    virtual QRectF boundingRect() const;
    void BoxItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent *event );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent *event );
    virtual void hoverMoveEvent (QGraphicsSceneHoverEvent * event);

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);

    void setGrabbers(qreal width, qreal height);
    void initContextMenu();
    GrabberID getSelectedGrabber(QPointF point);
    void setGrabberCursor(GrabberID stretchRectState);
    QRectF calculateMoveRect(QPointF dragStart, QPointF dragEnd);
    QRectF calculateStretchRect(QPointF dragStart, QPointF dragEnd);
    QRect getRealRect();
    void setTopmost();

    TaskStatus _taskStatus = Waiting;
    bool _isMouseMoved = false;

    QSize _imageSize;
    QRectF _sceneRect;
    QRectF _rect;

    QGraphicsTextItem _textRect, _textName;
    QRectF _boundingRect;
    QStringList _typeNameList;
    QString _typeName;

    QCursor _oldCursor;
    QMenu _contextMenu;
    QColor _color;
    qreal _penWidth = 2;
    QPen _pen;

    int _grabberWidth;
    int _grabberHeight;

    QRectF _grabbers[8];
    GrabberID _selectedGrabber;
    QPointF _dragStart, _dragEnd;

};

#endif // STATEBOX_H
