if(EXISTS "/mnt/c/Users/23324/Documents/Codes/cppjson/test/cppjsontest[1]_tests.cmake")
  include("/mnt/c/Users/23324/Documents/Codes/cppjson/test/cppjsontest[1]_tests.cmake")
else()
  add_test(cppjsontest_NOT_BUILT cppjsontest_NOT_BUILT)
endif()
