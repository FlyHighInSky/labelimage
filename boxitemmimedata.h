#ifndef BOXITEMMIMEDATA_H
#define BOXITEMMIMEDATA_H

#include <QObject>
#include <QGraphicsItem>
#include <QMimeData>
#include "boxitem.h"

class BoxItemMimeData : public QMimeData
{
    Q_OBJECT
public:
    BoxItemMimeData(QList<QGraphicsItem * > items);
    ~BoxItemMimeData();
    QList<QGraphicsItem *> items() const
    {
        return _itemList;
    }
private:
    QList<QGraphicsItem *> _itemList;
};

#endif // BOXITEMMIMEDATA_H
