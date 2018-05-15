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
** Summary: Companion class for the speedometer control Widget
**
****************************************************************************/

#include "radial_clock_plugin.h"
#include "radial_clock.h"

#include <QtPlugin>

#define mToolTip "A radial clock widget"
#define mWhatsThis "This widget represents a clock showing current time as a series of shrinking concentric rings"

RadialClockPlugin::RadialClockPlugin(QObject *parent) :
    QObject(parent), m_initialized(false)
{
}

//Q_EXPORT_PLUGIN2( RadialClockPlugin, RadialClockPlugin );

void RadialClockPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    if (m_initialized) return;
    m_initialized = true;
}

bool RadialClockPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget* RadialClockPlugin::createWidget(QWidget *parent)
{
    return new RadialClock(parent);
}

QString RadialClockPlugin::name() const
{
    return tr("Radial Clock");
}

QString RadialClockPlugin::group() const
{
    return tr("Funky");
}

QIcon RadialClockPlugin::icon() const
{
    return QIcon();
}

QString RadialClockPlugin::toolTip() const
{
    return tr(mToolTip);
}

QString RadialClockPlugin::whatsThis() const
{
    return tr(mWhatsThis);
}

bool RadialClockPlugin::isContainer() const
{
    return false;
}

QString RadialClockPlugin::includeFile() const {
    return "radial_clock.h";
}

QString RadialClockPlugin::domXml() const
{
    return  "<ui language=\"c++\">\n"
            " <widget class=\"RadialClock\" name=\"radial_clock\">\n"
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
