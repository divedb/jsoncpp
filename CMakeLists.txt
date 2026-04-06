cmake_minimum_required(VERSION 3.20)

project(jsoncpp
  LANGUAGES CXX
  VERSION 0.1.0 
  DESCRIPTION "A modern C++ JSON parser library."
)

set(JSONCPP_LICENSE "MIT")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -Iinclude
add_library(jsoncpp INTERFACE)
target_include_directories(jsoncpp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

option(JSONCPP_BUILD_TESTS "Build tests for jsoncpp" OFF)

if (JSONCPP_BUILD_TESTS)
# sudo apt install libgtest-dev
# cd /usr/src/gtest
# sudo cmake . && sudo make
# sudo cp lib/*.a /usr/lib/
# find_package(GTest REQUIRED)
# enable_testing()
# add_subdirectory(tests)

  enable_testing()
  include(FetchContent)
  set(BUILD_GMOCK OFF CACHE BOOL "Disable building gmock" FORCE)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG v1.15.2
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  add_subdirectory(tests)
endif()
