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
    m_imageFile("/home/pi/qt_widgets/QtWidgets/slide_puzzle/puzzle/image.png")
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
    const QList<QGraphicsItem*> &items = m_scene->items();
    for(QList<QGraphicsItem*>::const_iterator it(items.begin());
        it != items.end(); it++) {
        if ((*it)->type() != Tile::cType){
           continue;
        }
        static_cast<Tile*>(*it)->describe(strm);
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
    int d = thick/2;
    // Top and Bottom
    m_scene->addLine(d-dx,d-dy,width-d-dx,d-dy, borderPen);
    m_scene->addLine(d-dx,height-d-dy,width-d-dx,height-d-dy, borderPen);
    // Left and Right
    m_scene->addLine(d-dx,d-dy,d-dx,height-d-dy, borderPen);
    m_scene->addLine(width-d-dx,d-dy,width-d-dx,height-d-dy, borderPen);

    //describe(std::cout);
}

void SlidePuzzle::scramble()
{
    m_solved = false;
    const QRectF &rect = m_scene->sceneRect();

    QList<Tile*> tiles;
    QList<QGraphicsItem*> items = m_scene->items(rect);
    for(QList<QGraphicsItem*>::const_iterator it(items.begin());
        it != items.end(); it++) {
        QGraphicsItem *item = *it;
        if (item->type() != Tile::cType) continue;
        tiles.push_back(static_cast<Tile*>(item));
    }

    int dx = rect.x();
    int dy = rect.y();
    int width = rect.width();
    int height = rect.height();
    qsrand(QTime::currentTime().msec());

    for(int r = 0; r < m_rows; r++) {
        for(int c = 0; c < m_columns; c++) {
            int i = qrand() % tiles.size();
            QList<Tile*>::iterator it = tiles.begin() + i;
            Tile* tile = *it;
            tiles.erase(it);

            int w = tile->width();
            int h = tile->height();
            tile->setBorder(true);
            if (tiles.empty()) {
                tile->setPos(width-w,height-h);
                tile->setActive(false);
                tile->setEnabled(false);
            } else {
                tile->setActive(true);
                tile->setEnabled(true);
                tile->setPos(position(-dx, c, w), position(-dy, r, h));
            }
        }
    }

    m_scene->update();
}

void SlidePuzzle::validate()
{
    QList<QGraphicsItem*> items = m_scene->items(m_scene->sceneRect());
    QList<Tile*> tiles;

    Tile* missing = NULL;
    for(QList<QGraphicsItem*>::const_iterator it(items.begin());
        it != items.end(); it++) {
        QGraphicsItem *item = *it;
        if (item->type() != Tile::cType) continue;
        Tile *tile = static_cast<Tile*>(item);
        if (tile->valid() == false) {
            enable();
            return;
        }
        tiles.push_back(tile);
        if (tile->isEnabled() == false) {
            missing = tile;
        }
    }

    int dx = (missing->column()-missing->acolumn());
    int dy = (missing->row()-missing->arow());
    missing->shift(missing->width()*dx, missing->height()*dy, false);

    for(QList<Tile*>::const_iterator it(tiles.begin());
        it != tiles.end(); it++) {
        (*it)->setBorder(false);
        (*it)->setEnabled(false);
    }

    m_solved = true;
    m_scene->update();
    enable();
}

void SlidePuzzle::enable()
{
    button(this)->setEnabled(true);
}

void SlidePuzzle::disable()
{
    button(this)->setEnabled(false);
}
