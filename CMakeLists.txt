cmake_minimum_required(VERSION 3.14)

set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}
)

project(
  QSyncable
  LANGUAGES CXX
  VERSION ${${PROJECT_NAME}_VERSION})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/QSListModel
    ${CMAKE_CURRENT_SOURCE_DIR}/qslistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qspatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qstree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qstreenode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qsuuid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qsyncablefunctions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qsyncableqmltypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qsyncableqmlwrapper.cpp)
set(INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/qsdiffrunner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qsfastdiffrunner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qsjsonlistmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qslistmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qspatch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qspatchable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qsuuid.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qsyncablefunctions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qsyncableqmlwrapper.h)

set(INCLUDE_PRIV_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/priv/qsalgotypes_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/priv/qsdiffrunneralgo_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/priv/qsfastdiffrunneralgo_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/priv/qsimmutablewrapper_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/priv/qstree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/priv/qstreenode.h)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${INCLUDE_FILES}
                                   ${INCLUDE_PRIV_FILES})
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
# target_link_libraries(${PROJECT_NAME} PRIVATE project_options_and_warnings)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core
                                             Qt${QT_VERSION_MAJOR}::Qml)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)

# Examples
option(${PROJECT_NAME}_EXAMPLES "Enable Test Builds" ${IS_ROOT_PROJECT})

if(${PROJECT_NAME}_EXAMPLES)
  message(STATUS "Building Examples")
  add_subdirectory(examples/faketrello)
endif()

# Install script
option(${PROJECT_NAME}_INSTALL "Enable Installing as Library"
       ${IS_ROOT_PROJECT})

if(${PROJECT_NAME}_INSTALL)
  # Offer the user the choice of overriding the installation directories
  install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    EXPORT ${PROJECT_NAME}Targets)

  install(FILES ${INCLUDE_FILES} DESTINATION include/${PROJECT_NAME})
  install(FILES ${INCLUDE_PRIV_FILES} DESTINATION include/${PROJECT_NAME}/priv)

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  # installation - build tree specific package config files
  export(EXPORT ${PROJECT_NAME}Targets
         FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)
  # installation - relocatable package config files
  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION cmake)

  set(CONFIG_PACKAGE_LOCATION ${CMAKE_INSTALL_LIBDIR}/cmake/QuickFlux)

  install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CONFIG_PACKAGE_LOCATION})

  install(FILES ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
                ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Version.cmake
          DESTINATION ${CONFIG_PACKAGE_LOCATION})
endif()
