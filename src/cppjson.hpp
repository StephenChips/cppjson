#ifndef CPP_JSON
#define CPP_JSON

#include <string>

class JSON
{
public:
    bool isBool();
    bool isNumber();
    bool isString();
    bool isNull();
    bool isObject();
    bool isArray();

    JSON &operator[](std::string s);
    JSON &operator[](int idx);

    template <typename T>
    T get();
};

template <>
double JSON::get<double>();

template <>
bool JSON::get<bool>();

template <>
std::string JSON::get<std::string>();

std::string toString(JSON &json);
JSON parse(const std::string &str);

#endif