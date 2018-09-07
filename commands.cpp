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
        _boxList->append(box);
    }
}

void RemoveBoxesCommand::undo()
{
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        _scene->addItem(item);
    }
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_boxList, true);
}

void RemoveBoxesCommand::redo()
{
    for (int i=0; i<_boxList->count(); i++) {
        BoxItem *item = _boxList->at(i);
        _scene->removeItem(item);
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
    _oldName = _box->typeName();
    _newName = typeName;
}

void SetTargetTypeCommand::undo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_box, true);
    _box->setTypeName(_oldName);
}

void SetTargetTypeCommand::redo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_box, true);
    _box->setTypeName(_newName);
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
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_box, true);
    _box->setRect(_oldRect);
}

void MoveBoxCommand::redo()
{
    reinterpret_cast<ViewScene *>(_scene)->selectBoxItems(_box, true);
    _box->setRect(_newRect);
}
