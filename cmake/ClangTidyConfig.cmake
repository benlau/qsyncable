option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if(ENABLE_CLANG_TIDY)
  find_program(CLANGTIDY clang-tidy)
  if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY}
                             -extra-arg=-Wno-unknown-warning-option)
  else()
    message(WARNING "clang-tidy requested but executable not found")
  endif()
endif()
