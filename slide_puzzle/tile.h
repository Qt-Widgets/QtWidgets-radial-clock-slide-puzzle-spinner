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
** Summary: Header for the tile object
**
****************************************************************************/

#ifndef TILE_H
#define TILE_H

#include <QObject>
#include <QGraphicsItem>
#include <QTimeLine>
#include <QGraphicsItemAnimation>

#include <iostream>

class Tile : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Tile(int id, int row, int column, const QImage &image);
    ~Tile();

    static const int Type;
    virtual int type() const;

    /************************************************************************
    ** Encapsulated Properties
    ** - row -- The intended row of the tile (Read-Only).
    ** - column -- The intended column of the tile (Read-Only).
    ** - arow -- The actual row of the tile (Read-Only).
    ** - acolumn -- The actual column of the tile (Read-Only).
    ** - border -- Whether or not to draw a border around the tile.
    ** - width -- The width of the tile (Read-Only).
    ** - height -- The height of the tile (Read-Only).
    ** - center -- The center of the tile (Read-Only).
    ** - origin -- The origin of the tile (Read-Only).
    ** - valid -- Whether or not the tile is in the correct postion.
    ************************************************************************/
    int row() const { return m_row; }
    int column() const { return m_column; }
    int arow() const;
    int acolumn() const;

    bool border() const { return m_border; }
    void setBorder(bool b) { m_border = b; }

    int width() const;
    int height() const;
    QPointF center() const;
    QPointF origin() const;

    bool valid() const;

    QRectF boundingRect() const;

    std::ostream &describe(std::ostream &strm) const;

    void shift(int dx, int dy, QObject *o, const char *m);

    static void borderLines(const QRectF &rect, int d, QList<QLineF> &lines);

protected:
    /************************************************************************
    ** Graphic Callbacks
    ************************************************************************/
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void start();
    void stop();
    void done();

public slots:
    void cleanAnimation();

private:
    /************************************************************************
    ** Internal Variables.
    ************************************************************************/
    int m_id;
    bool m_border;
    int m_row;
    int m_column;
    QImage m_image;
    QTimeLine m_timeLine;
    QGraphicsItemAnimation *m_animation;

    QTimeLine *timeLine() { return &m_timeLine; }

    bool neighbor(int &xf, int &yf) const;
};

#endif // TILE_H
