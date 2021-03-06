#include "commands.h"

/******************************************************************************
** AddBoxCommand
*/

AddBoxCommand::AddBoxCommand(QGraphicsScene *scene, QList<BoxItem *> *boxList, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _boxList  = boxList;
//    _boxList = new QList<BoxItem *>();
//    for (int i=0; i<boxList.count(); i++) {
//        BoxItem *box = qgraphicsitem_cast<BoxItem *>(boxList.at(i));
//        _boxList->append(box);
//    }
}

AddBoxCommand::AddBoxCommand(QGraphicsScene *scene, BoxItem *box, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _box = box;
//    _boxList = new QList<BoxItem *>();
//    for (int i=0; i<boxList.count(); i++) {
//        BoxItem *box = qgraphicsitem_cast<BoxItem *>(boxList.at(i));
//        _boxList->append(box);
//    }
}

void AddBoxCommand::undo()
{
    if (_box) {
        _scene->removeItem(_box);
        QApplication::setOverrideCursor(_box->oldCursor());
    }
    if (_boxList) {
        for (int i=0; i<_boxList->count(); i++) {
            BoxItem *item = _boxList->at(i);
            _scene->removeItem(item);
            QApplication::setOverrideCursor(item->oldCursor());
        }
    }
}

void AddBoxCommand::redo()
{ 
    //    QCursor c = Qt::CrossCursor;
    //    _box->setOldCursor(c);
    if (_boxList) {
        for (int i=0; i<_boxList->count(); i++) {
            BoxItem *item = _boxList->at(i);
            if (!item->rect().isNull())
                _scene->addItem(item);
        }
        reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_boxList, true);
        //    for (int i=0; i<_boxList->count(); i++) {
        //        BoxItem *item = _boxList->at(i);
        //        if (!item->rect().isNull())
        //            _scene->addItem(item);
        //    }
//        reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_boxList, true);
    }

    if (_box) {
        if (!_box->rect().isNull()) {
            _scene->addItem(_box);
            _box->setSelected(true);
        }
    }
}

/******************************************************************************
** RemoveBoxItemCommand
*/

RemoveBoxesCommand::RemoveBoxesCommand(QGraphicsScene *scene, QList<QGraphicsItem *> boxList,
                                        QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _boxList = new QList<BoxItem *>();
    for (int i=0; i<boxList.count(); i++) {
        BoxItem *box = qgraphicsitem_cast<BoxItem *>(boxList.at(i));
        _boxList->append(box);
    }
}

void RemoveBoxesCommand::undo()
{
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        if (!item->rect().isNull())
            _scene->addItem(item);
    }
    reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_boxList, true);
}

void RemoveBoxesCommand::redo()
{
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        _scene->removeItem(item);
        QApplication::setOverrideCursor(item->oldCursor());
    }
}

/******************************************************************************
** SetTargetTypeCommand
*/

//SetTargetTypeCommand::SetTargetTypeCommand(QGraphicsScene *scene, BoxItem *box, const QString &typeName,
//                                             QUndoCommand *parent)
SetTargetTypeCommand::SetTargetTypeCommand(QGraphicsScene *scene, QList<BoxItem *> *boxList, const QString &typeName,
                                             QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
//    _box = box;
    _boxList = new QList<BoxItem *>();
    for (int i=0; i<boxList->count(); i++) {
        BoxItem *box = boxList->at(i);
        _boxList->append(box);
    }
    _oldNameList = new QStringList();
    for (int i=0; i<boxList->count(); i++) {
        _oldNameList->append(boxList->at(i)->typeName());
    }
//    _oldName = _box->typeName();
    _newName = typeName;
}

void SetTargetTypeCommand::undo()
{
//    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_box, true);
//    _box->setTypeName(_oldName);
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        item->setTypeName(_oldNameList->at(i));
    }
    reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_boxList, true);
}

void SetTargetTypeCommand::redo()
{
//    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_box, true);
//    _box->setTypeName(_newName);
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        item->setTypeName(_newName);
    }
    reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_boxList, true);
}

/******************************************************************************
** MoveBoxCommand
*/

MoveBoxCommand::MoveBoxCommand(QGraphicsScene *scene, BoxItem *box, const QRectF &newRect, const QRectF &oldRect,
                                             QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _box = box;
    _oldRect = oldRect;
    _newRect = newRect;
}

void MoveBoxCommand::undo()
{
    reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_box, true);
    _box->setRect(_oldRect);
}

void MoveBoxCommand::redo()
{
    reinterpret_cast<CustomScene *>(_scene)->selectBoxItems(_box, true);
    _box->setRect(_newRect);
}
