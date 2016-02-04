#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T23:48:50
#
#-------------------------------------------------

QT       += qml quick testlib

QT       -= gui

TARGET = qsyncableunittests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    qsyncabletests.cpp \
    benchmarktests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
    qsyncabletests.h \
    benchmarktests.h

include(vendor/vendor.pri)
include(../../qsyncable.pri)

DISTFILES += \
    tst_jsonModel.qml \
    tst_uuid.qml \
    ../../README.md
