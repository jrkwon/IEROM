#-------------------------------------------------
#
# Project created by QtCreator 2015-10-14T10:46:06
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Metadata
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Metadata.cpp \
    ../Settings/Settings.cpp \
    ../Mission/Mission.cpp

HEADERS += \
    Metadata.h \
    ../Settings/Settings.h \
    ../Mission/Mission.h

INCLUDEPATH += $$PWD/../Settings \
    $$PWD/../Mission
