option(ENABLE_INCLUDE_WHAT_YOU_USE
       "Enable static analysis with include-what-you-use" OFF)

if(ENABLE_INCLUDE_WHAT_YOU_USE)
  find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
  if(INCLUDE_WHAT_YOU_USE)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
  else()
    message(
      SEND_ERROR "include-what-you-use requested but executable not found")
  endif()
endif()
