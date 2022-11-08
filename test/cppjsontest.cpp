#include <gtest/gtest.h>
#include "../cppjson/cppjson.hpp"
#include <string>
#include <limits>
#include <vector>

// Demonstrate some basic assertions.
TEST(CppJSONTests, TestType)
{
  JSON jsonObj;
  JSON jsonNumber(3.0f);
  JSON jsonNull(nullptr);
  JSON jsonBoolean(true);

  EXPECT_TRUE(jsonNull.isNull());
  EXPECT_TRUE(jsonNull.type() == JSON::Null);

  EXPECT_TRUE(jsonNumber.isNumber());
  EXPECT_TRUE(jsonNumber.type() == JSON::Number);
  EXPECT_EQ(jsonNumber.getNumber(), 3.0f);

  EXPECT_TRUE(jsonBoolean.isBoolean());
  EXPECT_TRUE(jsonBoolean.type() == JSON::Bool);
  EXPECT_TRUE(jsonBoolean.getBool());

  EXPECT_TRUE(jsonObj.isObject());
  auto val = jsonObj.getObject();
  EXPECT_TRUE(val.size() == 0);
}

TEST(CppJSONTests, TestParsingObject)
{
  auto result = parse(R"(
    {
      "number": 3,
      "string": "hello",
      "boolean": true,
      "null": null    , 
      "array": [  1, 3, true  ], 
      "object": { "number": 3 }
    }
  )");

  EXPECT_TRUE(result.isObject());
  EXPECT_EQ(result["number"].getNumber(), 3);
  EXPECT_EQ(result["string"].getString(), "hello");
  EXPECT_EQ(result["boolean"].getBool(), true);
  EXPECT_TRUE(result["null"].isNull());

  auto &vec = result["array"].getArray();
  auto &obj = result["object"].getObject();

  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec[0].getNumber(), 1);
  EXPECT_EQ(vec[1].getNumber(), 3);
  EXPECT_EQ(vec[2].getBool(), true);

  EXPECT_EQ(obj.size(), 1);
  EXPECT_EQ(obj["number"].getNumber(), 3);

  result = parse("{}");
  EXPECT_EQ(result.getObject().size(), 0);

  EXPECT_NO_THROW(parse("{ \"\": 3}"));

  EXPECT_THROW(parse("{ \"number\":  }"), SyntaxError);
  EXPECT_THROW(parse("{ \"number\"  }"), SyntaxError);
  EXPECT_THROW(parse("{ \""), SyntaxError);
  EXPECT_THROW(parse("{ \"\\s\": 3 }"), SyntaxError);
  EXPECT_THROW(parse("{ 3: 3 }"), SyntaxError);
  EXPECT_THROW(parse("{ 'number': 3 }"), SyntaxError);
  EXPECT_THROW(parse("{ hello: 3 }"), SyntaxError);
  EXPECT_THROW(parse("{ \"hello\" 3 }"), SyntaxError);
  EXPECT_THROW(parse(" \"hello\": 3 }"), SyntaxError);
  EXPECT_THROW(parse("{ \"hello\": 3"), SyntaxError);
  EXPECT_THROW(parse("{ \"hello\": 3, }"), SyntaxError);
}

TEST(CppJSONTests, TestParsingArray)
{
  auto json = parse("[ null, 2, \"\", { \"hello\": true }]");

  auto vec = json.getArray();

  EXPECT_TRUE(vec[0].isNull());
  EXPECT_TRUE(vec[1].isNumber());
  EXPECT_DOUBLE_EQ(vec[1].getNumber(), 2.0);
  EXPECT_TRUE(vec[2].isString());
  EXPECT_EQ(vec[2].getString(), std::string(""));
  EXPECT_TRUE(vec[3].isObject());
  EXPECT_EQ(vec[3].getObject().size(), 1);
  EXPECT_EQ(vec[3]["hello"].getBool(), true);

  json = parse("[]");

  vec = json.getArray();
  EXPECT_EQ(vec.size(), 0);

  EXPECT_THROW(parse("["), SyntaxError);
  EXPECT_THROW(parse("[3,]"), SyntaxError);
  EXPECT_THROW(parse("[,,3,,,]"), SyntaxError);
  EXPECT_THROW(parse("3,3 ]"), SyntaxError);
}

TEST(CppJSONTests, TestParsingNumber)
{
  EXPECT_THROW(parse("0x3e"), SyntaxError);
  EXPECT_THROW(parse("0x3E"), SyntaxError);
  EXPECT_THROW(parse("0332"), SyntaxError);

  // EXPECT_THROW(parse(".12345"), SyntaxError);
  // EXPECT_THROW(parse("12345."), SyntaxError);
  // EXPECT_THROW(parse("12345.E10"), SyntaxError);

  EXPECT_EQ(parse("0").getNumber(), 0);
  EXPECT_EQ(parse("332.33").getNumber(), 332.33);
  EXPECT_EQ(parse("1.23E+10").getNumber(), 1.23E+10);

  EXPECT_EQ(parse("1.23E+1000").getNumber(), std::numeric_limits<double>::infinity());
  EXPECT_EQ(parse("1.23e-1000").getNumber(), 0);
  EXPECT_EQ(parse("-1.23e-1000").getNumber(), 0);
}

TEST(CppJSONTests, TestParsingString)
{
  EXPECT_EQ(parse(u8"\"\\uD83D\\uDE00\"").getString(), std::string(u8"😀"));
  EXPECT_EQ(parse(u8"\"\\u4f60\\u597d\"").getString(), std::string(u8"你好"));
  EXPECT_EQ(parse(u8"\"Sony \\u03b16400\"").getString(), std::string(u8"Sony α6400"));
  EXPECT_EQ(parse(u8"\"\\uDE00\\uD83D\\u0020\\u0020\\uD83D\"").getString(), std::string(u8"\xED\xB8\x80\xED\xA0\xBD\x20\x20\xED\xA0\xBD"));
  EXPECT_EQ(parse(u8"\"\\t\\r\\n\\f\\b\\\\\\/\"").getString(), std::string(u8"\t\r\n\f\b\\/"));
  EXPECT_EQ(parse(u8"\"你好\"").getString(), std::string("你好"));

  EXPECT_THROW(parse("\"ABC"), SyntaxError);
  EXPECT_THROW(parse("ABC\""), SyntaxError);
  EXPECT_THROW(parse("\"\"\""), SyntaxError);
  EXPECT_THROW(parse("\"\\s\""), SyntaxError);
  EXPECT_THROW(parse("\"(\\u8A)\""), SyntaxError);
  EXPECT_THROW(parse("\\U0020"), SyntaxError);
}

TEST(CppJSONTests, TestParsingBoolean)
{
  EXPECT_EQ(parse("true").getBool(), true);
  EXPECT_EQ(parse("false").getBool(), false);

  EXPECT_THROW(parse("ture"), SyntaxError);
}

TEST(CppJSONTests, TestParsingNull)
{
  EXPECT_TRUE(parse("null").isNull());
  EXPECT_EQ(parse("null").getNull(), nullptr);

  EXPECT_THROW(parse("nil"), SyntaxError);
}

TEST(CppJSONTests, TestToStringBool)
{
  EXPECT_EQ(toString(JSON(true)), std::string("true"));
  EXPECT_EQ(toString(JSON(false)), std::string("false"));
}

TEST(CppJSONTests, TestToStringString)
{
  JSON json = "hello, world";
  EXPECT_EQ(toString(json), std::string("hello, world"));

  json = u8"你好\r\n😀\f\t\b\v\x02";
  EXPECT_EQ(toString(json), std::string(u8"你好\\r\\n😀\\f\\t\\b\\u000b\\u0002"));
}

TEST(CppJSONTests, TestToStringNull)
{
  JSON json = nullptr;

  EXPECT_EQ(toString(json), "null");
}

TEST(CppJSONTests, TestToStringNumber)
{
  // Test cases
  // 1. positive number
  // 2. negative number
  // 3. super big number
  // 4. super small number
  // 5. infinity
  // 6. not a number

  std::vector<double> values = {
      0,
      123.321,
      -123.321,
      1e+100,
      -1e+100,
      1e-100,
      -1e-100};

  for (auto it = values.begin(); it != values.end(); it++)
  {
    JSON json = *it;
    std::string strVal = toString(json);
    EXPECT_EQ(std::stod(strVal), *it);
  }

  values = {
      std::numeric_limits<double>::infinity(),
      -std::numeric_limits<double>::infinity(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::signaling_NaN(),
      -std::numeric_limits<double>::quiet_NaN(),
      -std::numeric_limits<double>::signaling_NaN(),
  };

  for (auto it = values.begin(); it != values.end(); it++)
  {
    JSON json = *it;
    std::string strVal = toString(json);
    EXPECT_EQ(strVal, std::string("null"));
  }
}

TEST(CppJSONTests, TestToStringArray)
{
  JSON json = std::vector<JSON> {};

  EXPECT_EQ(toString(json), std::string("[]"));

  json = std::vector<JSON> { true, false, std::vector<JSON> { true, false } };

  EXPECT_EQ(toString(json), "[true,false,[true,false]]");

  json.getArray().push_back(json);

  EXPECT_EQ(toString(json), "[true,false,[true,false],[true,false,[true,false]]]");
}

TEST(CppJSONTests, TestToStringObject)
{
  JSON json;

  EXPECT_EQ(toString(json), std::string("{}"));

  auto &obj = json.getObject();

  json["true"] = true;
  json["false"] = false;

  auto actual = parse(toString(json));

  EXPECT_EQ(actual["true"].getBool(), true);
  EXPECT_EQ(actual["false"].getBool(), false);
  EXPECT_EQ(actual.getObject().size(), 2);
  
  json["json"] = json;

  actual = parse(toString(json));
  auto subobject = actual["json"];
  
  EXPECT_EQ(subobject["true"].getBool(), true);
  EXPECT_EQ(subobject["false"].getBool(), false);
  EXPECT_EQ(subobject.getObject().size(), 2);
}