#include <QPainter>
#include <QGraphicsScene>

#include "constants.h"
#include "gamecontroller.h"
#include "snake.h"

static const qreal SNAKE_SIZE = TILE_SIZE;

Snake::Snake(GameController &controller) :
    head(0, 0),
    growing(7),
    length(0),
    speed(5),
    moveDirection(NoMove),
    controller(controller)
{
}

//返回包住蛇的所有范围的一个矩形
QRectF Snake::boundingRect() const
{
    qreal minX = head.x();
    qreal minY = head.y();
    qreal maxX = head.x();
    qreal maxY = head.y();

    foreach (QPointF p, tail) {
        maxX = p.x() > maxX ? p.x() : maxX;
        maxY = p.y() > maxY ? p.y() : maxY;
        minX = p.x() < minX ? p.x() : minX;
        minY = p.y() < minY ? p.y() : minY;
    }

    QPointF tl = mapFromScene(QPointF(minX, minY));
    QPointF br = mapFromScene(QPointF(maxX, maxY));

    QRectF bound = QRectF(tl.x(),  // x
                          tl.y(),  // y
                          br.x() - tl.x() + SNAKE_SIZE,      // width
                          br.y() - tl.y() + SNAKE_SIZE       //height
                          );
    return bound;
}

//画出蛇的形状
QPainterPath Snake::shape() const
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    double temp=1-sqrt(2)/2;
    QVector<QPoint> points;
    points.append(QPoint(0,temp*SNAKE_SIZE));
    points.append(QPoint(0,SNAKE_SIZE-temp*SNAKE_SIZE));
    points.append(QPoint(temp*SNAKE_SIZE,SNAKE_SIZE));
    points.append(QPoint(SNAKE_SIZE-temp*SNAKE_SIZE,SNAKE_SIZE));
    points.append(QPoint(SNAKE_SIZE,SNAKE_SIZE-temp*SNAKE_SIZE));
    points.append(QPoint(SNAKE_SIZE,temp*SNAKE_SIZE));
    points.append(QPoint(SNAKE_SIZE-temp*SNAKE_SIZE,0));
    points.append(QPoint(temp*SNAKE_SIZE,0));

    path.addPolygon(QPolygon(points));
    //path.addRect(QRectF(0, 0, SNAKE_SIZE, SNAKE_SIZE));

    foreach (QPointF p, tail) {
        QPointF itemp = mapFromScene(p);
        path.addRect(QRectF(itemp.x(), itemp.y(), SNAKE_SIZE, SNAKE_SIZE));
    }

    return path;
}

//将蛇画在游戏界面上
void Snake::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    painter->fillPath(shape(), Qt::yellow);
    painter->restore();
}

void Snake::setMoveDirection(Direction direction)
{
    if (moveDirection == MoveLeft && direction == MoveRight)
        return;
    if (moveDirection == MoveRight && direction == MoveLeft)
        return;
    if (moveDirection == MoveUp && direction == MoveDown)
        return;
    if (moveDirection == MoveDown && direction == MoveUp)
        return;
    moveDirection = direction;
}

Snake::Direction Snake::currentDirection()
{
	return moveDirection;
}

//控制蛇的前进
void Snake::advance(int step)
{
    if (!step) {
        return;
    }
    if (tickCounter++ % speed != 0) {
        return;
    }
    if (moveDirection == NoMove) {
        return;
    }

    //当growing值大于0时，在蛇前进过程中会不断增添身体长度，直到growing减为0
    if (growing > 0) {
		QPointF tailPoint = head;
        tail << tailPoint;
        growing -= 1;
        length+=1;
    }
    else {
        tail.removeFirst();
        tail << head;
    }

    switch (moveDirection) {
        case MoveLeft:
            moveLeft();
            break;
        case MoveRight:
            moveRight();
            break;
        case MoveUp:
            moveUp();
            break;
        case MoveDown:
            moveDown();
            break;
    }

    setPos(head);

    //每次运动过后检查碰撞是否发生
    handleCollisions();
}

void Snake::moveLeft()
{
    head.rx() -= SNAKE_SIZE;
    if (head.rx() < -100) {
        head.rx() = 90;
    }
}

void Snake::moveRight()
{
    head.rx() += SNAKE_SIZE;
    if (head.rx() >= 100) {
        head.rx() = -100;
    }
}

void Snake::moveUp()
{
    head.ry() -= SNAKE_SIZE;
    if (head.ry() < -100) {
        head.ry() = 90;
    }
}

void Snake::moveDown()
{
    head.ry() += SNAKE_SIZE;
    if (head.ry() >= 100) {
        head.ry() = -100;
    }
}

//检测碰撞并对其进行处理
void Snake::handleCollisions()
{
    //先提取出所有碰撞的元素
    QList<QGraphicsItem *> collisions = collidingItems();

    //如果是撞到了墙壁
    foreach (QGraphicsItem *collidingItem, collisions) {
        if (collidingItem->data(GD_Type) == GO_Wall) {
            //这里加了一个补丁，并不优雅，可能降低了运行速度，但保证了不出现bug
            int hx=head.x()/10+10,hy=head.y()/10+10;
            if(controller.GetMapInformation(hx,hy)=='w'){
                controller.snakeAteWall();
                return;
            }
        }
    }

    //如果是吃到食物
    foreach (QGraphicsItem *collidingItem, collisions) {
        if (collidingItem->data(GD_Type) == GO_Food) {
            controller.snakeAteFood((Food *)collidingItem);
            growing += 1;
        }
    }

    //检查是否撞到了自己
    if (tail.contains(head)) {
        controller.snakeAteItself();
        return;
    }

    //如果是吃到了毒药
    foreach (QGraphicsItem *collidingItem, collisions) {
        if (collidingItem->data(GD_Type) == GO_toxin) {
            controller.snakeAtetoxin((toxin *)collidingItem);
        }
    }
}
