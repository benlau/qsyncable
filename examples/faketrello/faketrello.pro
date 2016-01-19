TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    appdelegate.cpp \
    card.cpp \
    list.cpp \
    board.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

include(../../qsyncable.pri)

HEADERS += \
    appdelegate.h \
    card.h \
    list.h \
    board.h

DISTFILES += \
    README.md
