#include <QPainter>

#include "constants.h"
#include "food.h"

static const qreal FOOD_RADIUS = 3.0;

//构造函数
Food::Food(qreal x, qreal y)
{
    setPos(x, y);//x和y用于指定食物的坐标
    setData(GD_Type, GO_Food);//添加额外数据信息
}

//返回以(x,y)为中心，以2*TILE_SIZE为边长的矩形
QRectF Food::boundingRect() const
{
    return QRectF(-TILE_SIZE,    -TILE_SIZE,
                   TILE_SIZE * 2, TILE_SIZE * 2 );
}

//绘制shape()函数中定义的路线
void Food::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();//保存画笔状态

    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(shape(), Qt::red);

    painter->restore();//保存画笔状态
}

QPainterPath Food::shape() const
{
    QPainterPath p;
    p.addEllipse(QPointF(TILE_SIZE / 2, TILE_SIZE / 2), FOOD_RADIUS, FOOD_RADIUS);
    return p;
}
