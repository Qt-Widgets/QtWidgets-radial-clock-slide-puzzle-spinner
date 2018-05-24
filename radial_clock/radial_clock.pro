#-------------------------------------------------
#
# Copyright (C) 2018 Brian Hill
# All rights reserved
#
# License Agreement
#
# This program is free software: you can distribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY of FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# Software Author: Brian Hill <brian.hill@glowfish.ca>
#
# Project created by QtCreator 2018-05-08T17:51:34
#
#-------------------------------------------------

QT += widgets designer

CONFIG += plugin release

TARGET = $$qtLibraryTarget($$TARGET)

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
TEMPLATE = lib

SOURCES += radial_clock.cpp \
    radial_clock_plugin.cpp

HEADERS  += radial_clock.h \
    radial_clock_plugin.h

DISTFILES += \
    radial_clock.json

OTHER_FILES += \
    radial_clock.json
