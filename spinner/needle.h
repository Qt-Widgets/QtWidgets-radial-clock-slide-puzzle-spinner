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
** Summary: Header for the needle object
**
****************************************************************************/

#ifndef NEEDLE_H
#define NEEDLE_H

#include <QObject>
#include <QGraphicsItem>
#include <QElapsedTimer>
#include <math.h>

#define C_CIRCLE 360

class Needle : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Needle(int width, int height);
    ~Needle();

    static const int cType;
    virtual int type() const;

    /************************************************************************
    ** Encapsulated Properties
    ** - width -- The width of the needle (Read-Only).
    ** - height -- The height of the needle (Read-Only).
    ** - revolution -- The distance of one revolution (Read-Only).
    ************************************************************************/
    int width() const { return m_width; }
    int height() const { return m_height; }
    qreal revolution() const { return m_revolution; }
    long int elapsed() const { return m_elapsed.elapsed(); }

    void reset();

    void whenStarted(QObject *o, const char *m);
    void whenStopped(QObject *o, const char *m);

    QRectF boundingRect() const;

protected:
    /************************************************************************
    ** Graphic Callbacks
    ************************************************************************/
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void advance(int phase);

private:
    /************************************************************************
    ** Internal Variables.
    ************************************************************************/
    int m_width;
    int m_height;
    qreal m_speed;
    qreal m_startAngle;
    int m_stopAngle;
    QElapsedTimer m_elapsed;
    qreal m_revolution;
    qreal m_acceleration;
    qreal m_distance;
    qreal m_stop;

    bool spin();

signals:
    void starting();
    void stopping();
};

#endif // NEEDLE_H
