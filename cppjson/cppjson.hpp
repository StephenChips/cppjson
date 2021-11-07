#ifndef CPP_JSON
#define CPP_JSON

#include <string>

struct JSONObjectEntry
{
    std::string key;
    JSON value;

    JSONObjectEntry(std::string _k, JSON _v) : key(_k), value(_v){};
    JSONObjectEntry(std::string _k, std::initializer_list<JSONObjectEntry> _v);
    JSONObjectEntry(double _k, JSON _v);
    JSONObjectEntry(double _k, std::initializer_list<JSONObjectEntry> _v);
};

class JSON
{
private:
    enum Type
    {
        BOOL,
        NUMBER,
        STRING,
        JSONNULL,
        OBJECT,
        ARRAY,
    };

    JSON(JSON::Type type);

public:
    JSON();
    JSON(std::initializer_list<JSONObjectEntry> list);
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

    JSON &JSON::operator=(const JSON &val);

    template <typename T>
    T get();

    size_t size();

    void push_back(const JSON &val);
    void push_front(const JSON &val);
    void erase(size_t idx);
    void erase(size_t start, size_t end);
    void erase(const std::string &s);

    static JSON array(std::initializer_list<JSON> entries);
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