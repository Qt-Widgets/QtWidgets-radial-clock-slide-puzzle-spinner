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
#include <QColormap>
#include <QTime>
#include <QFile>
#include <QDirIterator>
#include <math.h>

#include <QDebug>

#include <iostream>

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

QString getResource(QString n)
{
    QString ref(":/images/");
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString name = it.next();
        if (name.left(ref.size()) != ref) {
            continue;
        }
        if (name == n || name.right(name.size()-ref.size()) == n) {
            return name;
        }
    }

    qDebug() << "Image " << n << " not found";
    return QString(":/images/not-found.png");
}

} // end namespace

/************************************************************************
** Constructor/Destructor
************************************************************************/
SlidePuzzle::SlidePuzzle(QWidget *parent) :
    QWidget(parent),
    m_rows(3),
    m_columns(3),
    m_imageFile(":/images/logo.png"),
    m_puzzleBackground(Qt::gray),
    m_imageBackground(Qt::white)
{
    Q_INIT_RESOURCE(images);
    m_scene = new QGraphicsScene(this);

    QGraphicsView *view = new QGraphicsView(m_scene, this);
    view->setStyleSheet("background: transparent");
    view->setRenderHint(QPainter::Antialiasing, false);
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

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
    init(false);
}

void SlidePuzzle::setColumns(int c)
{
    m_columns = c;
    init(false);
}

void SlidePuzzle::setImage(QString f)
{    
    QFile file(f);
    if (file.exists() == false) {
        f = getResource(f);
    }
    m_imageFile = f;
    init(false);
}

void SlidePuzzle::setPuzzleBackground(const QColor &c)
{
    m_puzzleBackground = c;
    QGraphicsRectItem *b = background();
    if (b != NULL) {
        QBrush brush(c);
        b->setBrush(brush);
        m_scene->update();
    }
}

void SlidePuzzle::setImageBackground(const QColor &c)
{
    m_imageBackground = c;
    if (init(false)) m_scene->update();
}

QString SlidePuzzle::describe() const
{
    QString props;
    props += "rows: " + QString::number(rows()) + "\n";
    props += "columns: " + QString::number(columns()) + "\n";
    props += "imageFile: " + image() + "\n";
    props += "puzzleBackground: " + puzzleBackground().name() + "\n";
    props += "imageBackground: " + imageBackground().name() + "\n";
    return props;
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
    if (populated() == false) {
        setup();
        scramble();
    }
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

QGraphicsRectItem *SlidePuzzle::background() const
{
    QList<QGraphicsRectItem*> o;
    itemsByType<QGraphicsRectItem>(o, QGraphicsRectItem::Type);
    return (o.empty()) ? NULL : o.first();
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

void SlidePuzzle::setEnabledTiles(bool e)
{
    QList<Tile*> t;
    tiles(t);
    for(QList<Tile*>::const_iterator it(t.begin());
        it != t.end(); it++) {
        (*it)->setBorder(e);
        (*it)->setEnabled(e);
    }
}

bool SlidePuzzle::init(bool flag)
{
    if (!flag && populated() == false) return false;
    setup();
    scramble();
    fit();
    m_scene->update();
    return true;
}

void SlidePuzzle::fit() {
    view(this)->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void SlidePuzzle::setup()
{
    m_scene->clear();
    QImage img(m_imageFile);
    if (img.width() == 0) {
        return;
    }

    const QSize &size = img.size();
    QImage destination(size, QImage::Format_RGB32);
    destination.fill(imageBackground());
    QPainter p(&destination);
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    p.drawImage(0, 0, img);

    int width = size.width();
    int height = size.height();
    int w = ceil((width+0.0)/m_columns);
    int h = ceil((height+0.0)/m_rows);
    int dx = w*m_columns - width;
    int dy = h*m_rows - height;

    m_scene->setSceneRect(-dx, -dy, width + 2*w + 2*dx, height + 2*dy);
    m_scene->clear();

    QPen backPen(puzzleBackground());
    QBrush backBrush(puzzleBackground());
    m_scene->addRect(-dx, -dy, width+2*dx, height+2*dy, backPen, backBrush);

    int id = 0;
    for(int r = 0; r < m_rows; r++) {
        for(int c = 0; c < m_columns; c++) {
            QRect box(position(-dx, c, w),
                      position(-dy, r, h),
                      w, h);
            Tile* tile = new Tile(id++, r, c, destination.copy(box));
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
    setEnabledTiles(true);
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

    const QRectF &rect = m_scene->sceneRect();
    int dx = rect.x();
    int dy = rect.y();
    int w = missing->width();
    int h = missing->height();

    int sx = (position(dx, missing->column(), w)-missing->x());
    int sy = (position(dy, missing->row(), h)-missing->y());
    missing->shift(sx, sy, this, SLOT(pass()));

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
    setEnabledTiles(false);

    m_scene->update();
    enable();
}
