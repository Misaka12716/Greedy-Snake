#ifndef TOXIN_H
#define TOXIN_H

#include <QGraphicsItem>

class toxin : public QGraphicsItem
{
public:
    toxin(qreal x, qreal y);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    QPainterPath shape() const;
};


#endif
