#-------------------------------------------------
#
# Project created by QtCreator 2018-05-08T17:51:34
#
#-------------------------------------------------

QT       += core gui designer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadialClock
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    radialclockwidget.cpp \
    shapeitem.cpp

HEADERS  += dialog.h \
    radialclockwidget.h \
    shapeitem.h

FORMS    += dialog.ui
