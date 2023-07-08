#include "wall.h"
#include <QPainter>

#include "constants.h"

//构造函数
Wall::Wall(qreal x, qreal y)
{
    setPos(x, y);
    setData(GD_Type, GO_Wall);
}

//返回能包住所有墙图形的一个矩形
QRectF Wall::boundingRect() const
{
    return QRectF(-TILE_SIZE,    -TILE_SIZE,
                  TILE_SIZE * 2, TILE_SIZE * 2 );
}

//将画好的墙的轨迹显示在页面上
void Wall::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(shape(), Qt::darkGray);

    painter->restore();
}

//画出墙的轨迹并返回
QPainterPath Wall::shape() const
{
    QPainterPath p;
    p.addRect(QRectF(0, 0, 10, 10));
    return p;
}

