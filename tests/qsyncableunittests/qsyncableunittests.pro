QT       += qml quick testlib
QT       -= gui

TARGET = qsyncableunittests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    qsyncabletests.cpp \
    benchmarktests.cpp \
    integrationtests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
    qsyncabletests.h \
    benchmarktests.h \
    integrationtests.h

include(vendor/vendor.pri)
include(../../qsyncable.pri)

DISTFILES += \
    tst_jsonModel.qml \
    tst_uuid.qml \
    ../../README.md \
    SampleData1.qml \
    SampleData1.json
