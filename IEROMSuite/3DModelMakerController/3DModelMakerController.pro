QT += core
QT -= gui

TARGET = 3DModelMakerController
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../Metadata/Metadata.cpp \
    ../Settings/Settings.cpp \
    ../Mission/Mission.cpp \
    model3dmakercontroller.cpp

HEADERS += \
    ../Metadata/Metadata.h \
    ../Settings/Settings.h \
    ../Mission/Mission.h \
    model3dmakercontroller.h

INCLUDEPATH += $$PWD/../Settings \
    $$PWD/../Metadata \
    $$PWD/../Mission \
    $$PWD/../3DModelMaker

