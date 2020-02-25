cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 REQUIRED
    Core
    Qml
    )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON) # This is to find generated *.moc and *.h files in build dir

add_library(QSyncable OBJECT
    qsdiffrunner.cpp
    qspatch.cpp
    qslistmodel.cpp
    qsuuid.cpp
    qsdiffrunneralgo.cpp
    qstree.cpp
    qstreenode.cpp
    qsjsonlistmodel.cpp
    qsyncableqmltypes.cpp
    qsyncablefunctions.cpp
    qsyncableqmlwrapper.cpp
    )

target_link_libraries(QSyncable PUBLIC
    Qt5::Core
    Qt5::Qml
    )
