#ifndef COMMANDS_H
#define COMMANDS_H

//#include <QGraphicsScene>
#include <boxitem.h>
#include <QUndoCommand>
#include <customscene.h>

class AddBoxCommand : public QUndoCommand
{
public:
    AddBoxCommand(QGraphicsScene *scene, QList<BoxItem *> *boxList, QUndoCommand *parent = 0);
    AddBoxCommand(QGraphicsScene *scene, BoxItem *box, QUndoCommand *parent = 0);
    ~AddBoxCommand()
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
    BoxItem *_box = nullptr;
};

class RemoveBoxesCommand : public QUndoCommand
{
public:
    RemoveBoxesCommand(QGraphicsScene *scene, QList<QGraphicsItem *> boxList, QUndoCommand *parent = 0);
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
    SetTargetTypeCommand(QGraphicsScene *scene, QList<BoxItem *> *boxList, const QString &typeName,
                            QUndoCommand *parent = 0);

    ~SetTargetTypeCommand()
    {
        if (_boxList) {
            delete _boxList;
        }
        if (_oldNameList) {
            delete _oldNameList;
        }
    }
    void undo() override;
    void redo() override;

private:
    QGraphicsScene *_scene;
    QString _oldName, _newName;
    QList<BoxItem *> *_boxList = nullptr;
    QStringList *_oldNameList = nullptr;
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
