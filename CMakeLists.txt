cmake_minimum_required(VERSION 3.14)

project(QSyncable LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

# compiler options
include(cmake/StandardProjectSettingsConfig.cmake)

add_library(project_options_and_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerOptionsAndWarningsConfig.cmake)
set_project_options_and_warnings(project_options_and_warnings)

# enable doxygen
include(cmake/DoxygenConfig.cmake)

# project satinizers
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options_and_warnings)

# static analyzers
include(cmake/CppCheckConfig.cmake)
include(cmake/ClangTidyConfig.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Qml
  REQUIRED)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Qml
  REQUIRED)

set(SRC_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsdiffrunner.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsdiffrunneralgo.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsjsonlistmodel.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qslistmodel.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qspatch.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qstree.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qstreenode.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsuuid.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsyncablefunctions.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsyncableqmltypes.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/qsyncableqmlwrapper.cpp
)

add_library(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/priv)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Qml project_options_and_warnings)


option(ENABLE_EXAMPLES "Enable Test Builds" ON)

if(ENABLE_EXAMPLES)
  message(STATUS "Building Examples")
  add_subdirectory(examples/faketrello)
endif()

