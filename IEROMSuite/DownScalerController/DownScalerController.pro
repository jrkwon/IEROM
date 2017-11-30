QT += core
QT -= gui

TARGET = DownScalerController
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../Metadata/Metadata.cpp \
    ../Settings/Settings.cpp \
    ../Mission/Mission.cpp \
    downscalercontroller.cpp

HEADERS += \
    ../Metadata/Metadata.h \
    ../Settings/Settings.h \
    ../Mission/Mission.h \
    downscalercontroller.h \

INCLUDEPATH += $$PWD/../Settings \
    $$PWD/../Metadata \
    $$PWD/../Mission \
    $$PWD/../DownScaler
