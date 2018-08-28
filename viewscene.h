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
#include "FreeImage.h"

class ViewScene : public QGraphicsScene
{
public:
    ViewScene(QObject* parent = 0);

    void loadImage(QString path);
    double viewZoom() const;
    void setViewZoom(int w, int h);
    void setViewZoom(double zoom);
    void saveToFile(const QString& path);
    void clear();

    QImage *image;
    bool isImageLoaded = false;
    QList<QString> *classNameList = nullptr;
    bool isDrawing = false;
    void Undo();
    void Redo();

protected:
    void drawView();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyPressEvent(QKeyEvent *keyEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);
    void deleteBoxItems();
    void selectAllBoxItems(bool op);
private:
    QGraphicsPixmapItem *pixmapItem = nullptr;
    BoxItem* boxItem = nullptr;
    double zoomFactor = 1;
    QPointF _dragStart, _dragEnd;
    bool _isPanning = false;
    QPointF leftTopPoint;
    QPointF rightBottomPoint;
    QString _labelFilePath;
    void loadBoxItemsFromFile();
    void saveBoxItemsToFile();    
};
#endif // VIEWSCENE_H
