#-------------------------------------------------
#
# Project created by QtCreator 2015-10-14T16:13:11
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = SliceAreaDetectorController
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../Metadata/Metadata.cpp \
    ../Settings/Settings.cpp \
    ../Mission/Mission.cpp \
    SliceAreaPositionWriter.cpp \
    SliceAreaDetectorController.cpp

HEADERS += \
    ../Metadata/Metadata.h \
    ../Settings/Settings.h \
    ../Mission/Mission.h \
    SliceAreaPositionWriter.h \
    SliceAreaDetectorController.h

INCLUDEPATH += $$PWD/../Settings \
    $$PWD/../Metadata \
    $$PWD/../Mission \
    $$PWD/../SliceAreaDetector

