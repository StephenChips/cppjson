add_test( HelloTest.BasicAssertions /mnt/c/Users/23324/Documents/Codes/cppjson/test/build/hello_test [==[--gtest_filter=HelloTest.BasicAssertions]==] --gtest_also_run_disabled_tests)
set_tests_properties( HelloTest.BasicAssertions PROPERTIES WORKING_DIRECTORY /mnt/c/Users/23324/Documents/Codes/cppjson/test/build)
add_test( HelloTest2.BasicAssertions /mnt/c/Users/23324/Documents/Codes/cppjson/test/build/hello_test [==[--gtest_filter=HelloTest2.BasicAssertions]==] --gtest_also_run_disabled_tests)
set_tests_properties( HelloTest2.BasicAssertions PROPERTIES WORKING_DIRECTORY /mnt/c/Users/23324/Documents/Codes/cppjson/test/build)
set( hello_test_TESTS HelloTest.BasicAssertions HelloTest2.BasicAssertions)
