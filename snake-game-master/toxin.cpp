#include <QPainter>

#include "constants.h"
#include "toxin.h"

static const qreal FOOD_RADIUS = 3.0;

//构造函数
toxin::toxin(qreal x, qreal y)
{
    setPos(x, y);//x和y用于指定毒药的坐标
    setData(GD_Type, GO_toxin);//添加额外数据信息
}

//返回以(x,y)为中心，以2*TILE_SIZE为边长的矩形
QRectF toxin::boundingRect() const
{
    return QRectF(-TILE_SIZE,    -TILE_SIZE,
                  TILE_SIZE * 2, TILE_SIZE * 2 );
}

//绘制shape()函数中定义的路线
void toxin::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();//保存画笔状态

    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(shape(), Qt::green);//颜色

    painter->restore();//保存画笔状态
}

//返回毒药的路径（轮廓）
QPainterPath toxin::shape() const
{
    QPainterPath p;
    //在以(x,y)为左上角的矩形范围之中，圆心为T/2，半径为FOOD_RADIUS的圆
    p.addEllipse(QPointF(TILE_SIZE / 2, TILE_SIZE / 2), FOOD_RADIUS, FOOD_RADIUS);
    return p;
}
