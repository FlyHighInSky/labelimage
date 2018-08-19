#ifndef VIEWSCENE_H
#define VIEWSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QAction>
#include <QGraphicsView>
#include <QKeyEvent>
#include "UndoHistory.h"
#include "boxitem.h"

class ViewScene : public QGraphicsScene
{
public:
    ViewScene(QObject* parent = 0);

    void loadImage(QString path);
    void setMaxViewSize(int w, int h);
    double viewZoom() const;
    void setViewZoom(double zoom);
    void saveToFile(const QString& path);

    void resize(double zoom);
    void resize(int w, int h);

    void Undo();
    void Redo();

protected:
    void drawView();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    QImage *image = nullptr;
    QGraphicsPixmapItem *pixmapItem;
    BoxItem* boxToDraw;
    double previewZoom = 1;
    QPointF leftTopPoint;
    QPointF rightBottomPoint;
    QGraphicsRectItem* itemToDraw;
    bool isDrawing;
    UndoHistory<QImage*> history;

};
#endif // VIEWSCENE_H
