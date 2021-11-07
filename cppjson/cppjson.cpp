#include <string>
#include "cppjson.hpp"

bool JSON::isBool() { return false; }
bool JSON::isNumber() { return false; }
bool JSON::isString() { return false; }
bool JSON::isNull() { return false; }
bool JSON::isObject() { return false; }
bool JSON::isArray() { return false; }

JSON &JSON::operator[](std::string s) {}

JSON &JSON::operator[](int idx)
{
}

JSON &JSON::operator=(bool _b) {};
JSON &JSON::operator=()

template <>
bool JSON::get<bool>(){};

template <>
std::string JSON::get<std::string>(){};

std::string toString(JSON &json) { return ""; };
JSON parse(const std::string &str){};