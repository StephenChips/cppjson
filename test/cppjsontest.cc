#include <gtest/gtest.h>
#include "../cppjson/cppjson.hpp"

// Test creating an JSON object and check if it has the correct type.
TEST(CppJSONTest, CreateJSONObjects)
{
  JSON jsonObject;
  JSON jsonNumber(3.14159);
  JSON jsonBoolean(true);
  JSON jsonArray = JSON::array();
  JSON jsonNull(nullptr);

  EXPECT_TRUE(jsonObject.isObject());
  EXPECT_TRUE(jsonNumber.isNumber());
  EXPECT_TRUE(jsonBoolean.isBool());
  EXPECT_TRUE(jsonNull.isNull());
  EXPECT_TRUE(jsonArray.isArray());
}

// Test object and array assignments and erasing and inserting new values.
// Try getting new inserted value, check the size of the object.

// Test getting internal value from an JSON::JSON object
// Checking if it throws errors when the type doesn't match.
