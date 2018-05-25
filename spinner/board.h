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
** Summary: Header for the board object
**
****************************************************************************/

#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class Board : public QGraphicsItem
{
public:
    typedef QList<QPair<QString, QColor> > tagList;

    explicit Board(int radius, const tagList &tags);
    Board(int radius);
    Board(int outerRadius, int innerRadius);
    Board(int outerRadius, int innerRadius, const QColor &center);
    Board(int outerRadius, const QColor &center);
    void init();
    ~Board();

    static const QString cUnknown;
    static const QColor cBlack;

    /************************************************************************
    ** Encapsulated Properties
    ** - outerRadius -- The radius of the outer colored circle.
    ** - innerRadius -- The radius of the inner circle.
    ** - color -- The color of the inner circle.
    ** - tagList -- The list of tags and colors.
    ** - num -- The number of tags set.
    ************************************************************************/
    int outerRadius() const { return m_outerRadius; }
    void setOuterRadius(int r) { m_outerRadius = r; }

    int innerRadius() const { return m_innerRadius; }
    void setInnerRadius(int r) { m_innerRadius = r; }

    const QColor &center() const { return m_center; }
    void setCenter(const QColor &c) { m_center = c; }

    const tagList &tags() const { return m_tags; }
    const QString &tag(int i) const {
        return ( i < m_tags.size() ) ? m_tags[i].first : cUnknown;
    }
    const QColor &color(int i) const {
        return ( i < m_tags.size() ) ? m_tags[i].second : cBlack;
    }
    int num() const { return m_tags.size(); }
    void addTag(const QString &t, const QColor &c)
    {
        m_tags.push_back(qMakePair(t, c));
    }
    void setTags(const tagList &t) { m_tags = t; }
    void clearTags() { m_tags.clear(); }

    const QString &value(qreal a);

    QRectF boundingRect() const;

protected:
    /************************************************************************
    ** Graphic Callbacks
    ************************************************************************/
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    /************************************************************************
    ** Internal Variables.
    ************************************************************************/
    int m_innerRadius;
    int m_outerRadius;
    QColor m_center;
    tagList m_tags;
    QMap<int, int> m_regions;
};

#endif // BOARD_H
