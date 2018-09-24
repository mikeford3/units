cmake_minimum_required(VERSION 3.0)
project("Strong Typedef" CXX)
set(CMAKE_VERBOSE_MAKEFILE off)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(HPX_IGNORE_COMPILER_COMPATIBILITY ON)
if(WIN32)
  if(MSVC)
  elseif(CMAKE_COMPILER_IS_GNUCXX)
  else(MSVC)
    message(SEND_ERROR "Unsupported compiler")
  endif(MSVC)
else(WIN32)

endif(WIN32)


message("${CMAKE_CXX_COMPILER_ID}")
message("${CMAKE_CXX_COMPILER}")
message("${CMAKE_CXX_COMPILER_ARG1}")
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("clang")
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
  message("GCC")
ELSE()
  message("Compiler unknown")
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")


IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("debug mode")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  message("release")
ELSE()
  message("unknown mode")
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)


file(GLOB TEST "base_dimensions_test.cpp" "prefixes_test.cpp" "tagging_logic_test.cpp" "numeric_functions_test.cpp"
               "derived_dimensions_test.cpp" "quantity_test.cpp" "common_quantities_test.cpp")
message("Cmake extra flags: ${CMAKE_EXTRA_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic ${CMAKE_EXTRA_FLAGS}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")# -stdlib=libc++ -lc++abi") #-stdlib=libstdc++ -Wno-missing-braces")
  #set(HPX_DIR "/usr/local/lib/hpx_clang/lib/cmake/HPX")
  #include_directories(SYSTEM "/usr/local/lib/hpx_clang/include")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -fmax-errors=5")
  #set(HPX_DIR "/usr/local/lib/hpx_gcc/lib/cmake/HPX")
  #include_directories(SYSTEM "/usr/local/lib/hpx_gcc/include")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")





add_executable(Test ${TEST})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z")

