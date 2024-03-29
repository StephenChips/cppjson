cmake_minimum_required(VERSION 3.0.0)
project(cppjson VERSION 0.1.0)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(SRC
  ./cppjson/cppjson.cpp
  ./cppjson/parse.cpp
  ./cppjson/toString.cpp
)

add_library(
  cppjson SHARED
  ${SRC}
)

target_include_directories(cppjson PUBLIC ./cppjson)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE Bool "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  cppjsontest
  test/cppjsontest.cpp
  ${SRC}
)

target_link_libraries(
  cppjsontest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(cppjsontest)

