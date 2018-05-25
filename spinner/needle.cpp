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
** Summary: Top class for the spinner needle widget
**
****************************************************************************/

#include "needle.h"

#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <iostream>
#include <math.h>

#define BaseSpeed 15
#define VariantSpeed 10

/************************************************************************
** Constants
************************************************************************/
const int Needle::cType = QGraphicsItem::UserType+2;

/************************************************************************
** Constructor/Destructor
************************************************************************/
Needle::Needle(int width, int height) :
    m_width(width),
    m_height(height),
    m_revolution(2*M_PI*height/2)
{
    setToolTip("Spinning Needle");
    reset();
}

Needle::~Needle()
{
}

int Needle::type() const
{
    return cType;
}

void Needle::reset() {
    qsrand(QTime::currentTime().msec());
    m_speed = BaseSpeed + (qrand() % VariantSpeed);
    m_elapsed.start();
    m_startAngle = rotation();
    m_stopAngle = int(qrand() % C_CIRCLE);
    m_distance = m_revolution * (2 + m_stopAngle/(C_CIRCLE+0.0));
    m_acceleration = -pow(m_speed, 2)/(2 * m_distance);
    m_stop = -m_speed/m_acceleration;
}

void Needle::whenStarted(QObject *o, const char *m)
{
    connect(this, SIGNAL(starting()), o, m);
}

void Needle::whenStopped(QObject *o, const char *m)
{
    connect(this, SIGNAL(stopping()), o, m);
}

QRectF Needle::boundingRect() const {
    return QRectF(-m_width/2, -m_height/2, m_width, m_height);
}

void Needle::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    int W = m_width;
    int H = m_height;
    int top = -H/2;
    int bottom = -top;
    int left = -W/2;
    int right = -left;

    int cx = (left + right)/2;
    int thick = W/4;
    int tail = H/8;

    QPolygonF polygon;
    polygon << QPointF(cx, top);
    int dpoint = std::min(W/2,tail);
    polygon << QPointF(right, top+dpoint);
    polygon << QPointF(cx + thick/2, top+dpoint);   
    int dtail = std::min(W/2 - thick/2,tail);
    polygon << QPointF(cx + thick/2, bottom-tail-dtail);
    polygon << QPointF(right, bottom-tail);
    polygon << QPointF(right, bottom);
    polygon << QPointF(cx + thick/2, bottom-dtail);
    polygon << QPointF(cx - thick/2, bottom-dtail);
    polygon << QPointF(left, bottom);
    polygon << QPointF(left, bottom-tail);
    polygon << QPointF(cx - thick/2, bottom-tail-dtail);
    polygon << QPointF(cx - thick/2, top+dpoint);
    polygon << QPointF(left, top+dpoint);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);
    painter->drawPolygon(polygon);
}

void Needle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    emit starting();
}

void Needle::advance(int phase)
{
    if (!phase) return;

    if (spin() == false) {
        emit stopping();
    }
}

bool Needle::spin() {
    qreal t = elapsed()/10;
    if (t >= m_stop) {
        return false;
    }

    qreal dist = m_speed * t + 0.5 * m_acceleration * pow(t, 2);
    if (abs(dist-m_distance) < 1) {
        return false;
    }

    int ang = int(m_startAngle + (dist/m_revolution)*C_CIRCLE) % C_CIRCLE;
    setRotation(ang);
    return true;
}

