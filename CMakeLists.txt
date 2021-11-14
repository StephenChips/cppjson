cmake_minimum_required(VERSION 3.0.0)
project(cppjson VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(cppjson cppjson.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_subdirectory(cppjson)
add_subdirectory(test)

include(CPack)
