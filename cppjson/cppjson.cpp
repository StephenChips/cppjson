#include <string>

#include "cppjson.hpp"

JSON::JSON(){};

JSON::JSON(std::initializer_list<JSONObjectEntry> list){};

JSON::JSON(bool val){};

JSON::JSON(double val){};

JSON::JSON(std::string val){};

JSON::JSON(nullptr_t val){};

JSON::JSON(const JSON &val){};

bool JSON::isBool(){};

bool JSON::isNumber(){};
bool JSON::isString(){};
bool JSON::isNull(){};
bool JSON::isObject(){};
bool JSON::isArray(){};

JSON &JSON::operator[](const std::string &s){};

JSON &JSON::operator[](const std::string &s){};
JSON &JSON::operator[](size_t idx){};

JSON &JSON::operator=(const JSON &val){};

template <>
double JSON::get<double>(){};

template <>
bool JSON::get<bool>(){};

template <>
std::string JSON::get<std::string>(){};

template <>
nullptr_t JSON::get<nullptr_t>(){};

size_t JSON::size(){};

void JSON::push_back(const JSON &val){};
void JSON::push_front(const JSON &val){};
void JSON::erase(size_t idx){};
void JSON::erase(size_t start, size_t end){};
void JSON::erase(const std::string &s){};

JSON JSON::array(std::initializer_list<JSON> entries){};