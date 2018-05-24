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
# Project created by QtCreator 2018-05-15T16:21:27
#
#-------------------------------------------------

QT       += widgets designer

CONFIG += plugin release

TARGET = $$qtLibraryTarget($$TARGET)

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
TEMPLATE = lib

SOURCES += spinner.cpp \
    spinner_plugin.cpp \
    needle.cpp \
    board.cpp

HEADERS  += spinner.h \
    spinner_plugin.h \
    needle.h \
    board.h

DISTFILES += \
    spinner.json

OTHER_FILES += \
    spinner.json
