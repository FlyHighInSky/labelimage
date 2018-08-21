#include "viewscene.h"
#include <QtDebug>

ViewScene::ViewScene(QObject* parent):
    QGraphicsScene(parent),
    history(5)
{
    boxToDraw = 0;
    isDrawing = false;
}

void ViewScene::loadImage(QString path)
{
    history.push(new QImage(path));
    drawView();
}
void ViewScene::drawView()
{
//    QList<BoxItem> *itemsList = new QList<BoxItem>();
//    foreach (QGraphicsItem *item, this->items()) {
//        if (item->type() == QGraphicsItem::UserType+1) {
//                itemsList->append(*(qgraphicsitem_cast<BoxItem *>(item)));
//        }
//    }

//            (this->items());
//    this->clear();
    // Actual image layer.
    auto pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*history.current()));
    pixmapItem->setAcceptHoverEvents(true);
    pixmapItem->setScale(previewZoom);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);

      addItem(pixmapItem);
      // Reset scene size to make scrollbars disappear.
      QPoint zero(0, 0);
      QRect rect(zero, history.current()->size() * previewZoom);
      setSceneRect(rect);

    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            BoxItem *b = qgraphicsitem_cast<BoxItem *>(item);
            this->removeItem(b);
            b->setScale(this->sceneRect(), previewZoom);
            this->addItem(b);
        } else if (item->type() == QGraphicsPixmapItem::Type) {
            removeItem(item);
            addItem(pixmapItem);
        }
    }


    // Layer with decorative half-transparent border.
//	auto pixmapSize = image->pixmap().size();
//	auto border = new QGraphicsRectItem(0, 0, pixmapSize.width(), pixmapSize.height());
//	border->setScale(previewZoom);
//	border->setPen(QPen(QColor(0, 0, 0, 64), 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

    // Adds layers to scene.
//        addItem(border);

    // Reset scene size to make scrollbars disappear.
//	QPoint zero(0, 0);
//	QRect rect(zero, history.current()->size() * previewZoom);
//        viewScene->setSceneRect(rect);
//      views().first()->viewport();

}

void ViewScene::deleteBoxItems()
{
    foreach (QGraphicsItem *item, this->selectedItems()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            this->removeItem(item);
            delete item;
        }
    }
}

void ViewScene::selectAllBoxItems()
{
    // unselecting box items
    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            item->setSelected(true);
        }
    }
}
void ViewScene::deselectAllBoxItems()
{
    // unselecting box items
    foreach (QGraphicsItem *item, this->selectedItems()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            item->setSelected(false);
        }
    }
}

void ViewScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
            if (event->modifiers() == Qt::ShiftModifier) { // drawing box item
                deselectAllBoxItems();
                leftTopPoint = event->scenePos();
                isDrawing = true;
            } else if (event->modifiers() == Qt::ControlModifier) { // selecting multiple box items
                QGraphicsScene::mousePressEvent(event);
            } else { // selecting single box item
                deselectAllBoxItems();
                QGraphicsScene::mousePressEvent(event);
            }
    }
}

void ViewScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    if(isDrawing) {
//        if(!itemToDraw) {
//            itemToDraw = new QGraphicsRectItem;
//            this->addItem(itemToDraw);
//            itemToDraw->setPen(QPen(Qt::black, 3, Qt::SolidLine));
//        }
//        rightBottomPoint = event->scenePos();
//        QRect roi(qMin(rightBottomPoint.x(), leftTopPoint.x()),
//                  qMin(rightBottomPoint.y(), leftTopPoint.y()),
//                  qAbs(rightBottomPoint.x() - leftTopPoint.x()),
//                  qAbs(rightBottomPoint.y() - leftTopPoint.y()));
//        int l = sceneRect().left(), r = sceneRect().right(),
//            t = sceneRect().top(), b = sceneRect().bottom();
//        if (roi.right() < l || roi.left() > r ||
//            roi.top() > b || roi.bottom() < t )
//            return;

//        roi.setTop(qMax(t, roi.top()));
//        roi.setLeft(qMax(l, roi.left()));
//        roi.setBottom(qMin(b, roi.bottom()));
//        roi.setRight(qMin(r, roi.right()));

//        itemToDraw->setRect(roi);
//        itemToDraw->setFlag(QGraphicsItem::ItemIsSelectable, true);
//        itemToDraw->setFlag(QGraphicsItem::ItemIsMovable, true);
//    }
    if(isDrawing) {
        if(!boxToDraw) {
            boxToDraw = new BoxItem(this->sceneRect());
            addItem(boxToDraw);
            boxToDraw->setSelected(true);
        }
        rightBottomPoint = event->scenePos();
        QRect roi(qMin(rightBottomPoint.x(), leftTopPoint.x()),
                  qMin(rightBottomPoint.y(), leftTopPoint.y()),
                  qAbs(rightBottomPoint.x() - leftTopPoint.x()),
                  qAbs(rightBottomPoint.y() - leftTopPoint.y()));
        int l = sceneRect().left(), r = sceneRect().right(),
            t = sceneRect().top(), b = sceneRect().bottom();
        if (roi.right() < l || roi.left() > r ||
            roi.top() > b || roi.bottom() < t )
            return;

        roi.setTop(qMax(t, roi.top()));
        roi.setLeft(qMax(l, roi.left()));
        roi.setBottom(qMin(b, roi.bottom()));
        roi.setRight(qMin(r, roi.right()));

        boxToDraw->setRect(roi);
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void ViewScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    isDrawing = false;
    boxToDraw = 0;
    QGraphicsScene::mouseReleaseEvent(event);
}

void ViewScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete) {
        deleteBoxItems();
    } else if(event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier) {
        selectAllBoxItems();
    } else {
        QGraphicsScene::keyPressEvent(event);
    }
}

/**
 * @brief Scales image given amount of times.
 */
void ViewScene::resize(double zoom)
{
    history.push(new QImage(
                     history.current()->scaled(
                         history.current()->size() * zoom,
                         Qt::IgnoreAspectRatio,
                         Qt::SmoothTransformation
                         )));
    drawView();
}

/**
 * @brief Scales image by fixed size.
 */
void ViewScene::resize(int w, int h)
{
    history.push(new QImage(
                     history.current()->scaled(
                         w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation )
                     ));
    drawView();
}

/**
 * @brief Returns current preview zoom of image.
 */
double ViewScene::viewZoom() const
{
    return previewZoom;
}

/**
 * @brief Sets image preview zoom.
 */
void ViewScene::setViewZoom(double zoom)
{
    previewZoom = zoom;
    drawView();
}

/**
 * @brief Sets fixed size of image preview.
 */
void ViewScene::setMaxViewSize(int w, int h)
{
    auto size = history.current()->size();
    previewZoom = std::min(1.0 * w / size.width(), 1.0 * h / size.height());
    drawView();
}
