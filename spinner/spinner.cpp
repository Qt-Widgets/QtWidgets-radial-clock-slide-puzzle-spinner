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
** Summary: Top class for the spinner widget
**
****************************************************************************/

#include "spinner.h"

#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <iostream>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QTimer>
#include <QGridLayout>
#include <QEventLoop>

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

QLabel *label(const QWidget *w)
{
    return static_cast<QLabel*>(byType(w, "QLabel"));
}

} // end namespace

/************************************************************************
** Constructor/Destructor
************************************************************************/
Spinner::Spinner(QWidget *parent) :
    QWidget(parent),
    m_radius(150),
    m_labels("A,B,C,D,A,B,C,D"),
    m_colors("#FF0000,#00FF00,#0000FF,#FF00FF")
{        
    m_scene = new QGraphicsScene(this);
    int D = 2*m_radius;
    m_scene->setSceneRect(-m_radius, -m_radius, D, D);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), m_scene, SLOT(advance()));
    populateBoard();
    populateNeedle();

    QGraphicsView *view = new QGraphicsView(m_scene, this);
    view->setStyleSheet("background: transparent");
    view->setRenderHint(QPainter::Antialiasing, false);
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    view->fitInView(m_board, Qt::KeepAspectRatio);

    QPushButton *spin = new QPushButton("spin", this);
    spin->setGeometry(0,0,10,10);
    spin->setText("Spin");
    connect(spin, SIGNAL(clicked()), this, SLOT(spin()));

    QLabel *value = new QLabel("value", this);
    value->setGeometry(0,0,10,10);
    value->setText("");

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(view,0,0,-1,1);
    layout->addWidget(value, 0,1,1,1);
    layout->addWidget(spin, 1,1,1,1);
    setLayout(layout);

    return;
}

Spinner::~Spinner()
{
}

void Spinner::spin() {
    disable();
    m_needle->reset();
    m_timer->start(20);
}

const QString &Spinner::value() const {
    if (!m_board) return Board::cUnknown;

    if (button(this)->isEnabled() == false) {
        m_loop.exec();
    }

    qreal angle = m_needle->rotation();
    // Normalize the angle to the board frame of reference;
    if (angle > 45 && angle < 225) {
        int d = angle - 45;
        angle = (45 - d + 360) % 360;
    } else if (angle < 45) {
        int d = 45 - angle;
        angle = (d + 45 + 360) % 360;
    } else if (angle > 225) {
        int d = angle - 225;
        angle = (225 - d + 360) % 360;
    }

    return m_board->value(angle);
}

void Spinner::populateBoard() {
    m_board = new Board(m_radius, QColor(Qt::white));
    m_board->setPos(0, 0);
    setTags();
    m_scene->addItem(m_board);
}

void Spinner::populateNeedle() {
    int W = 2*radius()/4;
    int H = W*3;
    m_needle = new Needle(W, H);
    m_needle->setPos(0, 0);
    m_scene->addItem(m_needle);
    m_needle->whenStopped(m_timer, SLOT(stop()));
    m_needle->whenStarted(this, SLOT(disable()));
    m_needle->whenStarted(this, SLOT(spin()));
    m_needle->whenStopped(this, SLOT(enable()));
    m_needle->whenStopped(&m_loop, SLOT(quit()));
}

QColor toColor(const QString &s)
{
    Q_UNUSED(s);
    // If a HEX color code
    if (s[0] == '#') {
        bool ok;
        int r = s.mid(1,2).toInt(&ok, 16);
        int g = s.mid(3,2).toInt(&ok, 16);
        int b = s.mid(5,2).toInt(&ok, 16);
        return QColor(r, g, b);
    }
    return Board::cBlack;
}

const QColor &findColor(int index, const QStringList &colors,
                        const QString &label, std::map<QString, QColor> &cache)
{
    std::map<QString, QColor>::iterator it = cache.find(label);
    if (index < colors.size()) {
        if (it != cache.end()) {
            cache.erase(it);
        }
        QColor color = toColor(colors[index]);
        std::pair<std::map<QString, QColor>::iterator, bool> r =
            cache.insert(std::make_pair(label, color));
        return r.first->second;
    }
    if (it == cache.end()) {
        return Board::cBlack;
    }
    return it->second;
}

void Spinner::setTags()
{
    m_board->clearTags();
    std::map<QString, QColor> cache;
    QStringList labels = m_labels.split(',', QString::KeepEmptyParts);
    QStringList colors = m_colors.split(',', QString::KeepEmptyParts);
    int index = 0;
    for(QStringList::const_iterator lit(labels.begin());
        lit != labels.end(); lit++) {
        const QString &label = *lit;
        QColor color = findColor(index, colors, label, cache);
        m_board->addTag(label, color);
        index++;
    }
}

void Spinner::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    fit();
}

void Spinner::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    fit();
}

void Spinner::fit() {
    int D = 2*m_radius;
    view(this)->fitInView(-m_radius, -m_radius, D, D, Qt::KeepAspectRatio);
}

void Spinner::disable() {
    QPushButton *btn = button(this);
    btn->setEnabled(false);
    QLabel *val = label(this);
    val->setEnabled(false);
}

void Spinner::enable() {
    QPushButton *btn = button(this);
    btn->setEnabled(true);
    QLabel *val = label(this);
    val->setEnabled(true);
    val->setText(QString("Result: ") + value());

}
