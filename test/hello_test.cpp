#include <gtest/gtest.h>
#include "../cppjson/cppjson.hpp"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions)
{
  JSON jsonObj;
  JSON jsonNumber(3.0f);
  JSON jsonNull(nullptr);
  JSON jsonBoolean(false);

  EXPECT_TRUE(jsonObj.isObject());
}