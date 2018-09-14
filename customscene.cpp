#include "customscene.h"
#include <QtDebug>
#include <QScrollBar>

CustomScene::CustomScene(QObject* parent):
    QGraphicsScene(parent),
    _image(nullptr),
    _pixmapItem(nullptr),
    _boxItem(nullptr),
    _isDrawing(false),
    isImageLoaded(false),
    _undoStack(new QUndoStack)
{
}

void CustomScene::clear()
{
    saveBoxItemsToFile();
    isImageLoaded = false;

    if (_image != nullptr) {
        delete _image;
        _image = nullptr;
    }
    if (_pixmapItem != nullptr) {
        delete _pixmapItem;
        _pixmapItem = nullptr;
    }

    foreach (QGraphicsItem *item, this->items()) {
        if ((item->type() == QGraphicsItem::UserType+1) ||
                (item->type() == QGraphicsPixmapItem::Type) ) {
            this->removeItem(item);
            delete item;
        }
    }

    if (_undoStack != nullptr) {
        _undoStack->clear();
        delete _undoStack;
        _undoStack = nullptr;
    }
}

void CustomScene::loadImage(QString filename)
{
    //    image = new QImage(path);
    // Get image format
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.toStdString().c_str(), 0);
    if(fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename.toStdString().c_str());
    if(fif == FIF_UNKNOWN)
        return;

    // Load image if possible
    FIBITMAP *dib = nullptr;
    if(FreeImage_FIFSupportsReading(fif)) {
        dib = FreeImage_Load(fif, filename.toStdString().c_str());
        if(dib == nullptr)
            return;
    } else
        return;

    // Convert to 24bits and save to memory as JPEG
    FIMEMORY *stream = FreeImage_OpenMemory();
    // FreeImage can only save 24-bit highcolor or 8-bit greyscale/palette bitmaps as JPEG
    dib = FreeImage_ConvertTo24Bits(dib);
    FreeImage_SaveToMemory(FIF_JPEG, dib, stream);

    // Free memory
    FreeImage_Unload(dib);

    // Load JPEG data
    BYTE *mem_buffer = nullptr;
    DWORD size_in_bytes = 0;
    FreeImage_AcquireMemory(stream, &mem_buffer, &size_in_bytes);

    // Load raw data into QImage and return
    QByteArray array = QByteArray::fromRawData((char*)mem_buffer, size_in_bytes);

    //    image = QImage::fromData(array);
    _image = new QImage();
    _image->loadFromData(array);

    _pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*_image));
    _pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    this->addItem(_pixmapItem);

    emit imageLoaded(_image->size());

    setSceneRect(_image->rect());

    // load box items
    QFileInfo info(filename);
    _boxItemFileName = info.path() + "/" + info.completeBaseName() + ".txt";
    loadBoxItemsFromFile();
    isImageLoaded = true;
}

void CustomScene::loadBoxItemsFromFile()
{
    QFile file(_boxItemFileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QPoint zero(0, 0);
    QRect fatherRect(zero, _image->size());
    qreal x,y,w,h;
    int index;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList info = in.readLine().split(" ");
        if(info.size() >= 5) {
            index = info.at(0).toInt();
            w = info.at(3).toFloat() * _image->width();
            h = info.at(4).toFloat() * _image->height();
            x = info.at(1).toFloat() * _image->width() - w/2;
            y = info.at(2).toFloat() * _image->height() - h/2;

            BoxItem *b = new BoxItem(fatherRect, _image->size(), _typeNameList, _typeNameList.at(index));
            b->setRect(x,y,w,h);
            this->registerItem(b);
            this->addItem(b);
        }
    }
    file.close();
}

void CustomScene::registerItem(BoxItem *b)
{
    connect(b, SIGNAL(typeNameChanged(QString)), this, SLOT(changeBoxTypeName(QString)));
    connect(b, SIGNAL(boxSelected(QRect, QString)), this, SIGNAL(boxSelected(QRect, QString)));
    connect(b, SIGNAL(stretchCompleted(QRectF, QRectF)), this, SLOT(moveBox(QRectF, QRectF)));
    connect(b, SIGNAL(moveCompleted(QRectF, QRectF)), this, SLOT(moveBox(QRectF, QRectF)));
    b->installEventFilter(this);
}

void CustomScene::saveBoxItemsToFile()
{
    QFile file(_boxItemFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    qreal label[4];
    QTextStream out(&file);

    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            BoxItem *b = qgraphicsitem_cast<BoxItem *>(item);
            b->rect(label);
            QString s;
            s.sprintf("%d %f %f %f %f\n",
                      _typeNameList.indexOf(b->typeName()),
                      label[0],label[1],label[2],label[3]);
            out << s;
        }
    }
    file.close();
}

void CustomScene::deleteBoxItems()
{
    QList<BoxItem *> *boxList = new QList<BoxItem *>();
    foreach (QGraphicsItem *item, this->selectedItems()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            BoxItem *b = qgraphicsitem_cast<BoxItem *>(item);
            boxList->append(b);
        }
    }

    if (boxList->count() > 0)
        _undoStack->push(new RemoveBoxesCommand(this, boxList));
    delete boxList;
}

void CustomScene::selectBoxItems(bool op)
{
    // selecting box items
    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            item->setSelected(op);
        }
    }
}

void CustomScene::selectBoxItems(BoxItem *box, bool op)
{
    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            item->setSelected(item==qgraphicsitem_cast<QGraphicsItem *>(box));
        }
    }
    //    QGraphicsItem *item = this->items().at(index);
    //    item->setSelected(op);
}

void CustomScene::selectBoxItems(QList<BoxItem *> *boxList, bool op)
{
    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            item->setSelected(boxList->contains(qgraphicsitem_cast<BoxItem *>(item)));
        }
    }
}

void CustomScene::drawingBoxRect(bool op)
{
    _isDrawing = op;
    _isPanning = false;
    //    if (_isDrawing) {
    //        this->_pixmapItem->setCursor(Qt::CrossCursor);
    //    } else {
    //        this->_pixmapItem->setCursor(Qt::ArrowCursor);
    //    }
    //    if (_isDrawing) {
    //        _oldCursor = this->views().at(0)->viewport()->cursor();
    //        this->views().at(0)->viewport()->setCursor(Qt::CrossCursor);
    //    } else {
    //        this->views().at(0)->viewport()->setCursor(_oldCursor);
    //    }
    QCursor c = _isDrawing ? Qt::CrossCursor : Qt::ArrowCursor;
    //    _pixmapItem->setCursor(c);
//        if (_isDrawing) {
//            this->views().at(0)->viewport()->setCursor(Qt::CrossCursor);
//        } else {
//            this->views().at(0)->viewport()->setCursor(_oldCursor);
//        }


    foreach (QGraphicsItem *item, this->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            qgraphicsitem_cast<BoxItem *>(item)->setOldCursor(c);
        }
    }
}

//void ViewScene::panImage(bool op)
//{
//    _isPanning = op;
//    _isDrawing = false;
//    selectBoxItems(false);
////    if (_isPanning) {
////        _oldCursor = this->views().at(0)->viewport()->cursor();
////        this->views().at(0)->viewport()->setCursor(Qt::ClosedHandCursor);
////    } else {
////        this->views().at(0)->viewport()->setCursor(_oldCursor);
////    }
//}

//void ViewScene::setTopmost(QGraphicsItem *item)
//{
//    QList<QGraphicsItem *> list = collidingItems(item);
//    foreach (QGraphicsItem *it, list) {
//        it->stackBefore(item);
//    }
//}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->buttons()) {
    case Qt::LeftButton:
        //        if (event->modifiers() == Qt::ShiftModifier) { // drawing box item
        //            selectBoxItems(false); // deselect all the other box items
        //            _leftTopPoint = event->scenePos();
        //            _isDrawing = true;
        //        } else if (event->modifiers() == Qt::ControlModifier) { // selecting multiple box items
        //            QGraphicsScene::mousePressEvent(event);
        //        } else { // selecting single box item
        //            selectBoxItems(false);
        //            QGraphicsScene::mousePressEvent(event);
        //        }
        _leftTopPoint = event->scenePos();
        if (_isDrawing && event->modifiers() != Qt::ControlModifier) { // drawing box item
            //            this->views().at(0)->viewport()->setCursor(Qt::CrossCursor);

            // no box selected
            int count = 0;
            foreach (QGraphicsItem *item, this->selectedItems()) {
                if (item->type() == QGraphicsItem::UserType + 1) {
                    if (item->isSelected()) {
                        count++;
                    }
                }
            }
            if (count <= 0) {
                QGraphicsScene::mousePressEvent(event);
                return;
            }

            // selected box do not contain pressed pos
            count = 0;
            foreach (QGraphicsItem *item, this->items()) {
                if (item->type() == QGraphicsItem::UserType + 1) {
                    if (item->isSelected() && !item->contains(_leftTopPoint)) {
                        item->setSelected(false);
                        count++;
                    }
                }
            }
            if (count >0) {
                QGraphicsScene::mousePressEvent(event);
                return;
            }

            // selected box contain pressed pos
            foreach (QGraphicsItem *item, this->items()) {
                if ((item->type() == QGraphicsItem::UserType + 1 && item->contains(_leftTopPoint))) {
                    _isMoving = true;
                    QGraphicsScene::mousePressEvent(event);
                    return;
                }
            }
        } else {
            if (event->modifiers() == Qt::ControlModifier) { // selecting multiple box items
                foreach (QGraphicsItem *item, this->items()) {
                    if ((item->type() == QGraphicsItem::UserType + 1 && item->contains(_leftTopPoint))) {
                        item->setSelected(true);
                        break;
                    }
                }
                QGraphicsScene::mousePressEvent(event);
            } else if (_isPanning) { // pan image
                selectBoxItems(false);
                _dragStart = event->scenePos();
                return;
            } else {// selecting single box item
                foreach (QGraphicsItem *item, this->items()) {
                    if ((item->type() == QGraphicsItem::UserType + 1 && item->contains(_leftTopPoint))) {
                        selectBoxItems(qgraphicsitem_cast<BoxItem *>(item), true);
                        _isMoving = true;
                        QGraphicsScene::mousePressEvent(event);
                        break;
                    }
                }
            }
        }
        break;
    case Qt::RightButton:
        selectBoxItems(false);
        QGraphicsScene::mousePressEvent(event);
        break;
    default:
        break;
    }
    QGraphicsScene::mousePressEvent(event);
}

void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        _isMouseMoved = true;

        // add new box item
        if(_isDrawing && (this->selectedItems().count() <= 0 || _boxItem) && !_isMoving && !_isPanning) {
            //            this->views().at(0)->viewport()->setCursor(Qt::CrossCursor);
            if(!_boxItem) {
                _boxItem = new BoxItem(this->sceneRect(), _image->size(), _typeNameList, _typeName);
                this->registerItem(_boxItem);
                this->addItem(_boxItem);
            }
            _boxItem->setSelected(true);
            _rightBottomPoint = event->scenePos();
            QRect roi(qMin(_rightBottomPoint.x(), _leftTopPoint.x()),
                      qMin(_rightBottomPoint.y(), _leftTopPoint.y()),
                      qAbs(_rightBottomPoint.x() - _leftTopPoint.x()),
                      qAbs(_rightBottomPoint.y() - _leftTopPoint.y()));
            int l = sceneRect().left(), r = sceneRect().right(),
                    t = sceneRect().top(), b = sceneRect().bottom();
            if (roi.right() < l || roi.left() > r ||
                    roi.top() > b || roi.bottom() < t )
                return;

            roi.setTop(qMax(t, roi.top()));
            roi.setLeft(qMax(l, roi.left()));
            roi.setBottom(qMin(b, roi.bottom()));
            roi.setRight(qMin(r, roi.right()));

            _boxItem->setRect(roi);
        }

        if (_isMoving) {
            QGraphicsScene::mouseMoveEvent(event);
        }

        // pan scene
        if (_isPanning) {
//                        this->views().at(0)->viewport()->setCursor(Qt::ClosedHandCursor);
            QScrollBar *h = this->views().at(0)->horizontalScrollBar();
            h->setValue(h->value() - (int)(event->scenePos().x() - _dragStart.x()));
            QScrollBar *v = this->views().at(0)->verticalScrollBar();
            v->setValue(v->value() - (int)(event->scenePos().y() - _dragStart.y()));
            _dragEnd = event->scenePos();
            return;
        }
    }
    emit cursorMoved(event->scenePos());

    if(!(_isDrawing && this->selectedItems().count()<=0))
        QGraphicsScene::mouseMoveEvent(event);
}

void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (_isMouseMoved) {
        if (_isDrawing) {
            if (_boxItem) {
                this->removeItem(_boxItem);
                _undoStack->push(new AddBoxCommand(this, _boxItem));
                _boxItem = 0;
                return;
            }
        }
        if (_isPanning) {
            return;
        }

        if (_isMoving) {
            _isMoving = false;
            QGraphicsScene::mouseReleaseEvent(event);
        }

        _isMouseMoved = false;
        return;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void CustomScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->buttons() == Qt::LeftButton)
    {
        _isPanning = true;
        _dragStart = mouseEvent->scenePos();
        //        _oldCursor = this->views().at(0)->viewport()->cursor();
        //        this->views().at(0)->viewport()->setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void CustomScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_Delete) {
        deleteBoxItems();
    } else if(keyEvent->key() == Qt::Key_A && keyEvent->modifiers() == Qt::ControlModifier) {
        selectBoxItems(true);
    }else {
        QGraphicsScene::keyPressEvent(keyEvent);
    }
}

void CustomScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    QGraphicsScene::keyReleaseEvent(keyEvent);
}

//void ViewScene::setViewZoom(double zoom)
//{
//    _zoomFactor = zoom;
//    drawView();
//}

//void ViewScene::setViewZoom(int w, int h)
//{
//    QSize size = _image->size();
//    _zoomFactor = std::min(1.0 * w / size.width(), 1.0 * h / size.height());
//    drawView();
//}

void CustomScene::changeBoxTypeName(QString name)
{
    _typeName = name;
    QList<BoxItem *> *boxList = new QList<BoxItem *>();
    foreach (QGraphicsItem *item, this->selectedItems()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            BoxItem *b = qgraphicsitem_cast<BoxItem *>(item);
            boxList->append(b);
        }
    }

    if (boxList->count() > 0)
        _undoStack->push(new SetTargetTypeCommand(this, boxList, name));
    delete boxList;
}

void CustomScene::moveBox(QRectF newRect, QRectF oldRect)
{
    BoxItem *item = reinterpret_cast<BoxItem *>(QObject::sender());
    if (item != nullptr) {
        _undoStack->push(new MoveBoxCommand(this, item, newRect, oldRect));
    }
}
