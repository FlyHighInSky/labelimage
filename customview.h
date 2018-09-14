#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H

#include <QObject>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>

class CustomView : public QGraphicsView
{
    Q_OBJECT

public:
    CustomView(QObject* parent);
public slots:
    void updateCursor(bool checked);
private:
    QCursor _cursor;
protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // CUSTOMVIEW_H
