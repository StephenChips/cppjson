#ifndef CPP_JSON
#define CPP_JSON

#include <string>
#include <vector>
#include <map>

class JSON
{
private:
    enum JSONType
    {
        BOOL,
        NUMBER,
        STRING,
        JSONNULL,
        OBJECT,
        ARRAY,
    } valType;

    nullptr_t valNullptr = nullptr;
    std::string valStr;
    double valDouble;
    bool valBoolean;
    std::vector<JSON> valArray;
    std::map<std::string, JSON> valObject;

public:
    JSON();
    JSON(bool val);
    JSON(double val);
    JSON(std::string val);
    JSON(nullptr_t val);
    JSON(const JSON &val);

    bool isBool();
    bool isNumber();
    bool isString();
    bool isNull();
    bool isObject();
    bool isArray();

    JSON &operator[](const std::string &s);
    JSON &operator[](size_t idx);

    JSON &operator=(const JSON &val);

    template <typename T>
    T get();

    size_t size();

    void push_back(const JSON &val);
    void push_front(const JSON &val);
    void erase(size_t idx);
    void erase(size_t start, size_t end);
    void erase(const std::string &s);

    static JSON array();
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