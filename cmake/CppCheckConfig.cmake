option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK_BIN cppcheck)

  if(CPPCHECK_BIN)
    execute_process(
      COMMAND ${CPPCHECK_BIN} --version
      OUTPUT_VARIABLE CPPCHECK_VERSION
      ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(CPPCHECK_PROJECT_ARG
        "--project=${PROJECT_BINARY_DIR}/compile_commands.json")
    set(CPPCHECK_BUILD_DIR_ARG
        "--cppcheck-build-dir=${PROJECT_BINARY_DIR}/cppcheck"
        CACHE STRING "The build directory to use")
    # Don't show these errors
    if(EXISTS "${CMAKE_SOURCE_DIR}/.cppcheck_suppressions")
      set(CPPCHECK_SUPPRESSIONS
          "--suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck_suppressions"
          CACHE STRING "The suppressions file to use")
      message(STATUS "using suppressions ${CPPCHECK_SUPPRESSIONS}")
    else()
      set(CPPCHECK_SUPPRESSIONS
          ""
          CACHE STRING "The suppressions file to use")
    endif()

    # Show these errors but don't fail the build These are mainly going to be
    # from the "warning" category that is enabled by default later
    if(EXISTS "${CMAKE_SOURCE_DIR}/.cppcheck_exitcode_suppressions")
      set(CPPCHECK_EXITCODE_SUPPRESSIONS
          "--exitcode-suppressions=${CMAKE_SOURCE_DIR}/.cppcheck_exitcode_suppressions"
          CACHE STRING "The exitcode suppressions file to use")
    else()
      set(CPPCHECK_EXITCODE_SUPPRESSIONS
          ""
          CACHE STRING "The exitcode suppressions file to use")
    endif()

    set(CPPCHECK_ERROR_EXITCODE_ARG
        "--error-exitcode=1"
        CACHE STRING "The exitcode to use if an error is found")
    set(CPPCHECK_CHECKS_ARGS
        "--enable=all"
        CACHE STRING "Arguments for the checks to run")
    set(CPPCHECK_OUTPUT_TEMPLATE_ARGS
        "--template=gcc"
        CACHE STRING "Other arguments")

    set(CPPCHECK_ALL_ARGS
        ${CPPCHECK_PROJECT_ARG}
        ${CPPCHECK_BUILD_DIR_ARG}
        ${CPPCHECK_ERROR_EXITCODE_ARG}
        ${CPPCHECK_SUPPRESSIONS}
        ${CPPCHECK_EXITCODE_SUPPRESSIONS}
        ${CPPCHECK_CHECKS_ARGS}
        ${CPPCHECK_OUTPUT_TEMPLATE_ARGS})

    if(NOT CPPCHECK_XML_OUTPUT)
      set(CPPCHECK_COMMAND ${CPPCHECK_BIN} ${CPPCHECK_ALL_ARGS})
    else()
      set(CPPCHECK_COMMAND ${CPPCHECK_BIN} ${CPPCHECK_ALL_ARGS} --xml
                           --xml-version=2 2> ${CPPCHECK_XML_OUTPUT})
    endif()
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/cppcheck")
    add_custom_target(cppcheck_analysis COMMAND ${CPPCHECK_COMMAND} })
    message(STATUS "cppcheck found. Use cppccheck-analysis target to run it")
  else()
    message(WARNING "cppcheck not found")
  endif()
endif()
