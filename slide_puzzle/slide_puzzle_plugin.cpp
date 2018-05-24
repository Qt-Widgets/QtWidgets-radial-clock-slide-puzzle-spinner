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
** Summary: Companion class for the slide puzzle widget
**
****************************************************************************/

#include "slide_puzzle_plugin.h"
#include "slide_puzzle.h"

#include <QtPlugin>

#define mToolTip "A slide puzzle widget"
#define mWhatsThis "This widget represents a slide puzzle for assembling an image"

SlidePuzzlePlugin::SlidePuzzlePlugin(QObject *parent) :
    QObject(parent), m_initialized(false)
{
}

void SlidePuzzlePlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    if (m_initialized) return;
    m_initialized = true;
}

bool SlidePuzzlePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget* SlidePuzzlePlugin::createWidget(QWidget *parent)
{
    return new SlidePuzzle(parent);
}

QString SlidePuzzlePlugin::name() const
{
    return tr("Slide Puzzle");
}

QString SlidePuzzlePlugin::group() const
{
    return tr("Funky");
}

QIcon SlidePuzzlePlugin::icon() const
{
    return QIcon();
}

QString SlidePuzzlePlugin::toolTip() const
{
    return tr(mToolTip);
}

QString SlidePuzzlePlugin::whatsThis() const
{
    return tr(mWhatsThis);
}

bool SlidePuzzlePlugin::isContainer() const
{
    return false;
}

QString SlidePuzzlePlugin::includeFile() const {
    return "slide_puzzle.h";
}

QString SlidePuzzlePlugin::domXml() const
{
    return  "<ui language=\"c++\">\n"
            " <widget class=\"SlidePuzzle\" name=\"slide_puzzle\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "    <width>200</width>\n"
            "    <height>200</height>\n"
            "   </rect>\n"
            "  </property>\n"
            "  <property name=\"toolTip\" >\n"
            "   <string>" mToolTip "</string>\n"
            "  </property>\n"
            "  <property name=\"whatsThis\" >\n"
            "   <string>" mWhatsThis "</string>\n"
            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}
