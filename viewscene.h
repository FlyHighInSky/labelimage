#ifndef VIEWSCENE_H
#define VIEWSCENE_H

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

class ViewScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ViewScene(QObject* parent = 0);
    ~ViewScene()
    {
        if (isImageLoaded) {
            this->clear();
        }
    }

    void loadImage(QString path);
    double viewZoom() const;
    void setViewZoom(int w, int h);
    void setViewZoom(double zoom);
    void saveToFile(const QString& path);
    void clear();

    bool isImageLoaded = false;
    void setTypeNameList (const QStringList &list)
    {
        _typeNameList = list;
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
    void unregisterItem(BoxItem *b);

private slots:
    void changeTargetTypeName(QString name);
    void moveBox(QRectF rect);

signals:
    void imageLoaded(QSize imageSize);
    void cursorMoved(QPointF cursorPos);
    void boxSelected(QRect boxRect);

protected:
    void drawView();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyPressEvent(QKeyEvent *keyEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);
    void deleteBoxItems();
private:
    QImage *_image;
    QGraphicsPixmapItem *_pixmapItem = nullptr;
    BoxItem* _boxItem = nullptr;
    QStringList _typeNameList;
    double _zoomFactor = 1;
    QPointF _dragStart, _dragEnd;
    bool _isPanning = false;
    bool _isDrawing = false;
    QPointF _leftTopPoint;
    QPointF _rightBottomPoint;
    QString _labelFilePath;
    QUndoStack *_undoStack;
    void loadBoxItemsFromFile();
    void saveBoxItemsToFile();
};
#endif // VIEWSCENE_H
