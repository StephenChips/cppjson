#ifndef CPP_JSON
#define CPP_JSON

#include <string>
#include <vector>
#include <map>
#include <memory>

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
        UNPARSED,
        ABSENCE
    } type;

    struct Pos
    {
        const std::string *str;
        size_t start, end;

        Pos() = default;

        Pos(const std::string &str, size_t start, size_t end) : str(&str), start(start), end(end) {}
    };

    std::string valString;
    double valDouble;
    bool valBoolean;
    std::vector<JSON> valArray;
    std::map<std::string, JSON> valObject;
    struct Pos valPosition;

    JSON *ptrParentNode;
    
    std::unique_ptr<JSON> absenceNode;

    JSON(const JSON *ptrParentNode);

public:
    JSON();
    JSON(nullptr_t val);
    JSON(bool val);
    JSON(double val);
    JSON(std::string val);
    JSON(const JSON &val);
    JSON(const std::string &str, size_t start, size_t end);

    bool isBool();
    bool isNumber();
    bool isString();
    bool isNull();
    bool isObject();
    bool isArray();
    bool isAbsence();

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