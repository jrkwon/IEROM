QT += core




TARGET = VolumeMakerController
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../Metadata/Metadata.cpp \
    ../Settings/Settings.cpp \
    ../Mission/Mission.cpp \
    volumemakercontroller.cpp

HEADERS += \
    ../Metadata/Metadata.h \
    ../Settings/Settings.h \
    ../Mission/Mission.h \
    volumemakercontroller.h

INCLUDEPATH += $$PWD/../Settings \
    $$PWD/../Metadata \
    $$PWD/../Mission \
    $$PWD/../VolumeMaker
