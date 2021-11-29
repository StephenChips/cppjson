add_library(cppjson SHARED cppjson.cpp)

set(SRC ${SRC} cppjson.cpp)

target_include_directories(cppjson PUBLIC .)