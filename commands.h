#ifndef COMMANDS_H
#define COMMANDS_H

//#include <QGraphicsScene>
#include <boxitem.h>
#include <QUndoCommand>
#include <viewscene.h>

class AddBoxCommand : public QUndoCommand
{
public:
    AddBoxCommand(QGraphicsScene *scene, BoxItem *box, QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;

private:
    QGraphicsScene *_scene;
    BoxItem *_box;
};

class RemoveBoxesCommand : public QUndoCommand
{
public:
    RemoveBoxesCommand(QGraphicsScene *scene, QList<BoxItem *> *boxList, QUndoCommand *parent = 0);
    ~RemoveBoxesCommand()
    {
        if (_boxList) {
            delete _boxList;
        }
    }
    void undo() override;
    void redo() override;

private:
    QGraphicsScene *_scene;
    QList<BoxItem *> *_boxList = nullptr;
};

class SetTargetTypeCommand : public QUndoCommand
{
public:
    SetTargetTypeCommand(QGraphicsScene *scene, BoxItem *box, const QString &typeName,
                            QUndoCommand *parent = 0);

    void undo() override;
    void redo() override;

private:
    QGraphicsScene *_scene;
    QString _oldName, _newName;
    BoxItem *_box;
};

class MoveBoxCommand : public QUndoCommand
{
public:
    MoveBoxCommand(QGraphicsScene *scene, BoxItem *box, const QRectF &newRect, const QRectF &oldRect,
                                             QUndoCommand *parent=0);

    void undo() override;
    void redo() override;
private:
    QGraphicsScene *_scene;
    BoxItem *_box;
    QRectF _oldRect;
    QRectF _newRect;
};

#endif // COMMANDS_H
