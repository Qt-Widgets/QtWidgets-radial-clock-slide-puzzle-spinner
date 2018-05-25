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
** Summary: Header for the spinner widget
**
****************************************************************************/

#ifndef SPINNER_H
#define SPINNER_H

#include "needle.h"
#include "board.h"

#include <QWidget>
#include <QtDesigner/QDesignerExportWidget>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QEventLoop>

class QDESIGNER_WIDGET_EXPORT Spinner : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString labels READ labels WRITE setLabels);
    Q_PROPERTY(QString colors READ colors WRITE setColors);
    Q_PROPERTY(int innerRadius READ innerRadius WRITE setInnerRadius);
    Q_PROPERTY(int outerRadius READ outerRadius WRITE setOuterRadius);
    Q_PROPERTY(QColor center READ center WRITE setCenter);

public:
    explicit Spinner(QWidget *parent = 0);
    ~Spinner();

    /************************************************************************
    ** Encapsulated Properties
    ** - labels -- The set of labels, comma delimited.
    ** - colors -- The corresponding colors, comma delimited.
    ** - innerRadius -- The inner radius for the spinner plate.
    ** - outerRadius -- The outer radius of the spinner plate.
    ** - center -- The center color for inside the inner radius.
    ************************************************************************/
    int outerRadius() const { return m_radius; }
    void setOuterRadius(int r) {
        m_radius = r;
        updateRadius();
    }

    const QString &labels() const { return m_labels; }
    void setLabels(const QString &l) {
        m_labels = l;
        if (m_board) {
            setTags();
            update();
        }
    }

    const QString &colors() const { return m_colors; }
    void setColors(const QString &c) {
        m_colors = c;
        if (m_board) {
            setTags();
            update();
        }
    }

    int innerRadius() const
    {
        return m_board->innerRadius();
    }
    void setInnerRadius(int i)
    {
        m_board->setInnerRadius(i);
    }

    const QColor &center() const
    {
        return m_board->center();
    }
    void setCenter(const QColor &c) {
        m_board->setCenter(c);
    }

    void setTags();

    const QString &value() const;

    QString describe() const;

    void populateBoard();
    void populateNeedle();

protected:
    /************************************************************************
    ** Widget Callbacks
    ************************************************************************/
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

private:
    /************************************************************************
    ** Internal Variables.
    ************************************************************************/
    int m_radius;
    QString m_labels;
    QString m_colors;
    QGraphicsScene *m_scene;
    QTimer *m_timer;
    mutable QEventLoop m_loop;
    Needle *m_needle;
    Board *m_board;
    QGraphicsDropShadowEffect m_effect;

    void fit();
    void updateRadius();

public slots:
    void spin();
    void disable();
    void enable();
};

#endif // SPINNER_H
