#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T23:48:50
#
#-------------------------------------------------

QT       += qml quick testlib

QT       -= gui

TARGET = tst_qsyncableunitteststest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    qsyncabletests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
    qsyncabletests.h

include(vendor/vendor.pri)
include(../../qsyncable.pri)
