#-------------------------------------------------
#
# Project created by QtCreator 2015-10-17T15:43:09
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Mission
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Mission.cpp

HEADERS += \
    Mission.h

DISTFILES += \
    Mission.json

INCLUDEPATH += $$PWD/../Settings
