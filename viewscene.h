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

    QImage *image = nullptr;
    QList<QString> *classNameList = nullptr;
    void Undo();
    void Redo();

protected:
    void drawView();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void deleteBoxItems();
    void selectAllBoxItems(bool op);
private:
    QGraphicsPixmapItem *pixmapItem = nullptr;
    BoxItem* boxItem = nullptr;
    double zoomFactor = 1;
    QPointF leftTopPoint;
    QPointF rightBottomPoint;
    bool isDrawing = false;
    QString _labelFilePath;
    void loadBoxItemsFromFile();
    void saveBoxItemsToFile();
    bool _isLabelFileLoaded = false;
};
#endif // VIEWSCENE_H
