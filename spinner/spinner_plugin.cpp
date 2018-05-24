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
** Summary: Companion class for the spinner widget
**
****************************************************************************/

#include "spinner_plugin.h"
#include "spinner.h"

#include <QtPlugin>

#define mToolTip "A spinner widget"
#define mWhatsThis "This widget represents a spinner plate with a pointer needle"

SpinnerPlugin::SpinnerPlugin(QObject *parent) :
    QObject(parent), m_initialized(false)
{
}

void SpinnerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    if (m_initialized) return;
    m_initialized = true;
}

bool SpinnerPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget* SpinnerPlugin::createWidget(QWidget *parent)
{
    return new Spinner(parent);
}

QString SpinnerPlugin::name() const
{
    return tr("Spinner Plate");
}

QString SpinnerPlugin::group() const
{
    return tr("Funky");
}

QIcon SpinnerPlugin::icon() const
{
    return QIcon();
}

QString SpinnerPlugin::toolTip() const
{
    return tr(mToolTip);
}

QString SpinnerPlugin::whatsThis() const
{
    return tr(mWhatsThis);
}

bool SpinnerPlugin::isContainer() const
{
    return false;
}

QString SpinnerPlugin::includeFile() const {
    return "spinner.h";
}

QString SpinnerPlugin::domXml() const
{
    return  "<ui language=\"c++\">\n"
            " <widget class=\"Spinner\" name=\"spinner\">\n"
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
