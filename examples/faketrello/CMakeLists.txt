cmake_minimum_required(VERSION 3.14)

project(Faketrello LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_BINARY_DIR}/imports CACHE STRING "" FORCE)

message(STATUS "QML_IMPORT_PATH: ${QML_IMPORT_PATH}")

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Quick Qml
  REQUIRED)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Quick Qml
  REQUIRED)

find_package(QSyncable REQUIRED)

set(SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/appdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/appdelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/board.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/board.h
    ${CMAKE_CURRENT_SOURCE_DIR}/card.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/card.h
    ${CMAKE_CURRENT_SOURCE_DIR}/list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/list.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc
    )

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                              Qt${QT_VERSION_MAJOR}::Quick
                                              QSyncable)
