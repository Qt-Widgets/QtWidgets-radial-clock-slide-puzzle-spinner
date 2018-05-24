/****************************************************************************
**
** Copyright (C) 2018 Brian Hill
** All rights reserved.
**
** License Agreement
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** Software Author: Brian Hill <brian.hill@glowfish.ca>
**
** Summary: Top class for the tile object
**
****************************************************************************/

#include "tile.h"

#include <QPainter>
#include <QTimeLine>
#include <QGraphicsScene>
#include <QGraphicsItemAnimation>
#include <QGraphicsSceneMouseEvent>

/************************************************************************
** Constants
************************************************************************/
const int Tile::Type = QGraphicsItem::UserType+1;

/************************************************************************
** Constructor/Destructor
************************************************************************/
Tile::Tile(int id, int row, int column, const QImage &image) :
    m_id(id),
    m_row(row),
    m_column(column),
    m_image(image),
    m_animation(0)
{
    setActive(true);
    setVisible(true);

    m_timeLine.setDuration(500);
}

Tile::~Tile()
{
}

int Tile::type() const
{
    return Type;
}

int Tile::arow() const
{
    int dy = scene()->sceneRect().y();
    return (this->y()-dy)/height();
}

int Tile::acolumn() const {
    int dx = scene()->sceneRect().x();
    return (this->x()-dx)/width();
}

int Tile::width() const
{
    return m_image.size().width();
}

int Tile::height() const
{
    return m_image.size().height();
}

QPointF Tile::center() const
{
    return QPointF(this->x() + width()/2, this->y() + height()/2);
}

QPointF Tile::origin() const
{
    return QPointF(this->x(), this->y());
}

bool Tile::valid() const
{
    return (isEnabled() == false || (arow() == row() && acolumn() == column()));
}

QRectF Tile::boundingRect() const
{
    return QRectF(0, 0, width(), height());
}

std::ostream &Tile::describe(std::ostream &strm) const
{
    int x = this->x();
    int y = this->y();
    QPointF c = this->center();
    QRectF b = this->boundingRect();
    b.translate(x, y);
    strm << "Tile (" << this << ")" << " "
         << "[" << row() << "/" << column() << "] [" << arow() << "/" << acolumn() << "]" << std::endl
         << "  Border: " << b.x() << "," << b.y() << " - " << b.right() << "," << b.bottom() << std::endl
         << "  X, Y: " << x << "," << y << " (" << c.x() << "," << c.y() << ")" << std::endl;
    return strm;
}

void Tile::shift(int dx, int dy, QObject *obj, const char *slot)
{
    QTimeLine *tl = timeLine();
    tl->disconnect(tl, 0, 0, 0);
    tl->connect(tl, SIGNAL(finished()), this, SLOT(cleanAnimation()));

    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation();
    animation->setTimeLine(tl);
    animation->setItem(this);
    if (obj) connect(tl, SIGNAL(finished()), obj, slot);

    QPointF o = origin();
    animation->setPosAt(0, o);
    animation->setPosAt(1.0, o + QPointF(dx, dy));

    tl->start();
}

void Tile::borderLines(const QRectF &rect, int d, QList<QLineF> &lines)
{
    // Top line
    lines.append(QLineF(rect.x()+d, rect.y()+d, rect.right()-d, rect.y()+d));
    // Right line
    lines.append(QLineF(rect.right()-d, rect.y()+d, rect.right()-d, rect.bottom()-d));
    // Bottom line
    lines.append(QLineF(rect.right()-d, rect.bottom()-d, rect.x()+d, rect.bottom()-d));
    // Left line
    lines.append(QLineF(rect.x()+d, rect.bottom()-d, rect.x()+d, rect.y()+d));
}

void Tile::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    int xf = 0, yf = 0;
    if (neighbor(xf, yf) == false) {
        return;
    }

    emit start();
    shift(xf*width(), yf*height(), this, SIGNAL(stop()));
}

void Tile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawImage(0, 0, m_image);
    if (border()) {  
        QPen pen(Qt::black);
        int thick = std::min(width(), height())/30;
        pen.setWidth(thick);
        painter->setPen(pen);
        QList<QLineF> lines;
        QRectF box = boundingRect();
        borderLines(box, thick/2, lines);
        for(QList<QLineF>::const_iterator it(lines.begin());
            it != lines.end(); it++) {
            painter->drawLine(*it);
        }
    }
}

bool Tile::neighbor(int &xf, int &yf) const
{
    QGraphicsScene *scene = this->scene();
    int w = width();
    int h = height();
    int x = this->x() + w/2;
    int y = this->y() + h/2;
    QTransform transform;

    const QRectF &border = scene->sceneRect();
    int L = border.x();
    int R = L + border.width() - 2*w;
    int T = border.y();
    int B = T + border.height();

    for(xf = -1; xf < 2; xf ++) {
        for(yf = -1; yf < 2; yf ++) {
            if (xf == 0 && yf == 0) continue;
            if (xf != 0 && yf != 0) continue;
            if (xf == -1 && (x - w) < L) continue;
            if (yf == -1 && (y - h) < T) continue;
            if (xf == 1 && (x + w) > R) continue;
            if (yf == 1 && (y + h) > B) continue;
            QList<QGraphicsItem*> items = scene->items(QPointF(x + xf*w, y + yf*h));
            if (items.empty()) {
                return true;
            }
            bool found = false;
            for(QList<QGraphicsItem*>::const_iterator it(items.cbegin());
                it != items.cend(); it++) {
                QGraphicsItem* item = *it;
                if (item->type() == Tile::Type) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return true;
            }
        }
    }

    return false;
}

void Tile::cleanAnimation()
{
    if (m_animation) delete m_animation;
}
