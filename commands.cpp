#include "commands.h"

/******************************************************************************
** AddBoxCommand
*/

AddBoxCommand::AddBoxCommand(QGraphicsScene *scene, BoxItem *box, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _box = box;
}

void AddBoxCommand::undo()
{
    _scene->removeItem(_box);
}

void AddBoxCommand::redo()
{
    _scene->addItem(_box);
    _box->setSelected(true);
}

/******************************************************************************
** RemoveBoxItemCommand
*/

RemoveBoxesCommand::RemoveBoxesCommand(QGraphicsScene *scene, QList<BoxItem *> *boxList,
                                        QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _boxList = new QList<BoxItem *>();
    for (int i=0; i<boxList->count(); i++) {
        BoxItem *box = boxList->at(i);
        reinterpret_cast<ViewScene *>(_scene)->registerItem(box);
        _boxList->append(box);
    }
}

void RemoveBoxesCommand::undo()
{
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        _scene->addItem(item);
        item->setSelected(true);
    }
}

void RemoveBoxesCommand::redo()
{
    foreach (QGraphicsItem *item, _scene->selectedItems()) {
        if (item->type() == QGraphicsItem::UserType+1) {
            _scene->removeItem(item);
        }
    }
}

/******************************************************************************
** SetTargetTypeCommand
*/

SetTargetTypeCommand::SetTargetTypeCommand(QGraphicsScene *scene, BoxItem *box, const QString &typeName,
                                             QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _box = box;
    reinterpret_cast<ViewScene *>(_scene)->registerItem(_box);
    _oldName = _box->typeName();
    _newName = typeName;
    _index = _scene->items().indexOf(box);
}

void SetTargetTypeCommand::undo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_index, true);
    _box->setTypeName(_oldName);
}

void SetTargetTypeCommand::redo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_index, true);
    _box->setTypeName(_newName);
}

/******************************************************************************
** MoveBoxCommand
*/

MoveBoxCommand::MoveBoxCommand(QGraphicsScene *scene, BoxItem *box,const QRectF &rect,
                                             QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _scene = scene;
    _box = box;
    _oldRect = box->_originalRect;
    _newRect = rect;
    _index = _scene->items().indexOf(box);
}

void MoveBoxCommand::undo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_index, true);
    _box->setRect(_oldRect);
}

void MoveBoxCommand::redo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_index, true);
    _box->setRect(_newRect);
}
