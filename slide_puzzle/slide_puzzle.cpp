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
** Summary: Top class for the slide puzzle widget
**
****************************************************************************/

#include "slide_puzzle.h"
#include "tile.h"

#include <QGraphicsView>
#include <QPushButton>
#include <QGridLayout>
#include <QTime>
#include <math.h>

namespace {

QObject *byType(const QWidget *w, const std::string &className)
{
    const QList<QObject*> &children = w->children();
    for(QList<QObject*>::const_iterator it = children.begin();
        it != children.end(); it++) {
        if (strcmp((*it)->metaObject()->className(), className.c_str()) == 0) {
            return *it;
        }
    }
    return NULL;
}

QGraphicsView *view(const QWidget *w)
{
    return static_cast<QGraphicsView*>(byType(w, "QGraphicsView"));
}

QPushButton *button(const QWidget *w)
{
    return static_cast<QPushButton*>(byType(w, "QPushButton"));
}

int position(int offset, int multiple, int delta)
{
    return offset + multiple*delta;
}

} // end namespace

/************************************************************************
** Constructor/Destructor
************************************************************************/
SlidePuzzle::SlidePuzzle(QWidget *parent) :
    QWidget(parent),
    m_rows(2),
    m_columns(2),
    m_imageFile("/home/pi/qt_widgets/QtWidgets/slide_puzzle/image.png")
{
    m_scene = new QGraphicsScene(this);

    QGraphicsView *view = new QGraphicsView(m_scene, this);
    view->setStyleSheet("background: transparent");
    view->setRenderHint(QPainter::Antialiasing, false);
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    init();

    QPushButton *button = new QPushButton("scramble", this);
    button->setText("Reset");
    connect(button, SIGNAL(clicked()), this, SLOT(scramble()));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(view,0,0,-1,1);
    layout->addWidget(button,1,1,1,1);
    setLayout(layout);
}

void SlidePuzzle::setRows(int r)
{
    m_rows = r;
    init();
}

void SlidePuzzle::setColumns(int c)
{
    m_columns = c;
    init();
}

void SlidePuzzle::setImage(const QString& f)
{
    m_imageFile = f;
    init();
    fit();
}

std::ostream &SlidePuzzle::describe(std::ostream &strm) const
{
    QList<Tile*> t;
    tiles(t);
    for(QList<Tile*>::const_iterator it(t.begin());
        it != t.end(); it++) {
        (*it)->describe(strm);
    }
    return strm;
}

void SlidePuzzle::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    fit();
}

void SlidePuzzle::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    fit();
}

void SlidePuzzle::borders(QList<QGraphicsLineItem*> &l) const
{
    itemsByType<QGraphicsLineItem>(l, QGraphicsLineItem::Type);
}

void SlidePuzzle::tiles(QList<Tile *> &t) const
{
    itemsByType<Tile>(t, Tile::Type);
}

void SlidePuzzle::setBorderColor(const QColor &c)
{
    const QRectF &rect = m_scene->sceneRect();

    QList<QGraphicsLineItem*> b;
    borders(b);
    QPen pen(c);
    int d = std::min(rect.width(), rect.height());
    for(QList<QGraphicsLineItem*>::const_iterator it(b.begin());
        it != b.end(); it++) {
        QGraphicsLineItem *lineItem = *it;
        const QLineF &line = lineItem->line();
        if (line.dx() == 0) {
            d = std::min(d, int(std::min(line.x1()-rect.left(),
                                         rect.right()-line.x1())));
        } else {
            d = std::min(d, int(std::min(line.y1()-rect.top(),
                                         rect.bottom()-line.y1())));
        }
    }

    pen.setWidth(d*2);

    for(QList<QGraphicsLineItem*>::const_iterator it(b.begin());
        it != b.end(); it++) {
        QGraphicsLineItem *lineItem = *it;
        lineItem->setPen(pen);
    }
}

void SlidePuzzle::init()
{
    setup();
    scramble();
    fit();
}

void SlidePuzzle::fit() {
    view(this)->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void SlidePuzzle::setup()
{
    QImage img(m_imageFile);
    if (img.width() == 0) {
        return;
    }

    const QSize &size = img.size();
    int width = size.width();
    int height = size.height();
    int w = ceil((width+0.0)/m_columns);
    int h = ceil((height+0.0)/m_rows);
    int dx = w*m_columns - width;
    int dy = h*m_rows - height;

    m_scene->setSceneRect(-dx, -dy, width + 2*w + 2*dx, height + 2*dy);
    m_scene->clear();

    QPen backPen(Qt::white);
    QBrush backBrush(Qt::white);
    m_scene->addRect(-dx, -dy, width+2*dx, height+2*dy, backPen, backBrush);

    int id = 0;
    for(int r = 0; r < m_rows; r++) {
        for(int c = 0; c < m_columns; c++) {
            QRect box(position(-dx, c, w),
                      position(-dy, r, h),
                      w, h);
            Tile* tile = new Tile(id++, r, c, img.copy(box));
            tile->setPos(position(-dx, c, w), position(-dy, r, h));
            m_scene->addItem(tile);
            connect(tile, SIGNAL(stop()), this, SLOT(validate()));
            connect(tile, SIGNAL(start()), this, SLOT(disable()));
        }
    }

    QPen borderPen(Qt::black);
    int thick = std::min(width,height)/50;
    borderPen.setWidth(thick);
    QRectF box(-dx, -dy, width + 2*dx, height + 2*dy);
    QList<QLineF> lines;
    Tile::borderLines(box, thick/2, lines);
    for(QList<QLineF>::const_iterator it(lines.begin());
        it != lines.end(); it++) {
        m_scene->addLine(*it, borderPen);
    }
}

void SlidePuzzle::reset()
{
    QColor black(Qt::black);
    setBorderColor(black);

    QList<Tile*> t;
    tiles(t);
    for(QList<Tile*>::const_iterator it(t.begin());
        it != t.end(); it++) {
        (*it)->setBorder(true);
        (*it)->setEnabled(true);
    }
}


void SlidePuzzle::scramble()
{
    m_solved = false;
    reset();

    const QRectF &rect = m_scene->sceneRect();

    QList<Tile*> myTiles;
    tiles(myTiles);
    if (myTiles.empty()) return;

    int dx = rect.x();
    int dy = rect.y();
    int width = rect.width();
    qsrand(QTime::currentTime().msec());

    for(int r = 0; r < m_rows; r++) {
        for(int c = 0; c < m_columns; c++) {
            int i = qrand() % myTiles.size();
            QList<Tile*>::iterator it = myTiles.begin() + i;
            Tile* tile = *it;
            myTiles.erase(it);

            int w = tile->width();
            int h = tile->height();
            tile->setBorder(true);
            if (myTiles.empty()) {
                tile->setPos(width-w, position(dy, m_rows-1, h));
                tile->setActive(false);
                tile->setEnabled(false);
            } else {
                tile->setActive(true);
                tile->setEnabled(true);
                tile->setPos(position(dx, c, w), position(dy, r, h));
            }
        }
    }

    //describe(std::cout);
    m_scene->update();
}

void SlidePuzzle::validate()
{
    QList<QGraphicsItem*> items = m_scene->items(m_scene->sceneRect());

    Tile* missing = NULL;
    QList<Tile*> t;
    tiles(t);
    for(QList<Tile*>::const_iterator it(t.begin());
        it != t.end(); it++) {
        Tile *tile = *it;
        if (tile->valid() == false) {
            enable();
            return;
        }
        if (tile->isEnabled() == false) {
            missing = tile;
        }
    }

    int dx = (missing->column()-missing->acolumn());
    int dy = (missing->row()-missing->arow());
    missing->shift(missing->width()*dx, missing->height()*dy, this, SLOT(pass()));

    m_solved = true;    
}

void SlidePuzzle::enable()
{
    button(this)->setEnabled(true);
}

void SlidePuzzle::disable()
{
    button(this)->setEnabled(false);
}

void SlidePuzzle::pass()
{
    QColor green(Qt::green);
    setBorderColor(green);

    QList<Tile*> t;
    tiles(t);
    for(QList<Tile*>::const_iterator it(t.begin());
        it != t.end(); it++) {
        (*it)->setBorder(false);
        (*it)->setEnabled(false);
    }

    m_scene->update();
    enable();
}
