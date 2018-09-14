#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QAction>
#include <QGraphicsView>
#include <QKeyEvent>
#include "boxitem.h"
#include <QFileInfo>
#include <QFile>
#include <QImageReader>
#include <QUndoStack>
#include "commands.h"
#include "FreeImage.h"

class CustomScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CustomScene(QObject* parent = 0);
    ~CustomScene()
    {
        if (_image) {
            this->clear();
        }
    }

    void loadImage(QString path);
    void saveToFile(const QString& path);
    void clear();

    void setTypeNameList (const QStringList &list)
    {
        _typeNameList = list;
    }
    void setTypeName (const QString &name)
    {
        _typeName = name;
    }
    QUndoStack *undoStack() const
    {
        return _undoStack;
    }
    void undo();
    void redo();
    void selectBoxItems(QList<BoxItem *> *boxList, bool op);
    void selectBoxItems(BoxItem *box, bool op);
    void selectBoxItems(bool op);
    void registerItem(BoxItem *b);
    void drawBoxItem(bool op);
    void panImage(bool op);

public slots:
    void changeBoxTypeName(QString name);
private slots:
    void moveBox(QRectF newRect, QRectF oldRect);

signals:
    void imageLoaded(QSize imageSize);
    void cursorMoved(QPointF cursorPos);
    void boxSelected(QRect boxRect, QString typeName);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyPressEvent(QKeyEvent *keyEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);
    void deleteBoxItems();
//    void setTopmost(QGraphicsItem *item);
//    bool eventFilter(QObject *obj, QEvent *event);
private:
    QImage *_image;
    QGraphicsPixmapItem *_pixmapItem = nullptr;
    BoxItem* _boxItem = nullptr;//, *_selectedBoxItem;
    QString _typeName;
    QStringList _typeNameList;
    double _zoomFactor = 1;
    QPointF _dragStart, _dragEnd;
    bool _isPanning = false;
    bool _isDrawing = false;
    bool _isMoving = false;
    bool _isMouseMoved = false;
    QPointF _leftTopPoint;
    QPointF _rightBottomPoint;
    QString _boxItemFileName;
    QUndoStack *_undoStack;
//    QCursor _oldCursor = Qt::ArrowCursor;
//    QCursor _cursor;
    void loadBoxItemsFromFile();
    void saveBoxItemsToFile();
};
#endif // CUSTOMSCENE_H
