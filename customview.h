#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H

#include <QObject>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QScrollBar>
#include <QMouseEvent>

class CustomView : public QGraphicsView
{
    Q_OBJECT

public:
    CustomView(QObject* parent);
    void panImage(bool op);
public slots:
    void updateCursor(bool checked);
private:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    QCursor _cursor;
    bool _isPanning = false;
    bool _startFlag = false;
    int _panStartX, _panStartY;
};

#endif // CUSTOMVIEW_H
