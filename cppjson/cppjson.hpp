#ifndef CPP_JSON
#define CPP_JSON

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>

class SyntaxError : std::logic_error
{
public:
    SyntaxError() : std::logic_error("JSON syntax error") {}
};

class JSON
{
public:
    enum Type
    {
        Bool,
        Number,
        String,
        Null,
        Object,
        Array,
    };

private:
    Type _type;

    std::string valString;
    double valNumber;
    bool valBoolean;
    std::vector<JSON> valArray;
    std::map<std::string, JSON> valObject;

    std::unique_ptr<JSON> absenceNode;
    std::function<void()> setParentNodeFn;

    JSON(std::function<void()> setParentCallback) : setParentNodeFn(setParentCallback){};

public:
    JSON();
    JSON(std::string val);
    JSON(std::map<std::string, JSON> val);
    JSON(std::vector<JSON> val);
    JSON(const char *str);
    JSON(const JSON &val);
    JSON(nullptr_t val);
    JSON(bool val);
    JSON(double val);
    JSON(long val);
    JSON(JSON &&rhs)
    noexcept;

    bool isBoolean() const;
    bool isNumber() const;
    bool isString() const;
    bool isNull() const;
    bool isObject() const;
    bool isArray() const;

    Type type() const;

    JSON &operator[](const std::string &s);
    JSON &operator[](size_t idx);

    JSON &operator=(const JSON val);

    bool &getBool();
    const bool &getBool() const;

    double &getNumber();
    const double &getNumber() const;

    nullptr_t getNull() const;

    std::string &getString();
    const std::string &getString() const;

    std::vector<JSON> &getArray();
    const std::vector<JSON> &getArray() const;

    std::map<std::string, JSON> &getObject();
    const std::map<std::string, JSON> &getObject() const;

    size_t size() const;

    static JSON array();
    static JSON array(size_t sz);

    friend void swap(JSON &first, JSON &second);
};

std::string toString(const JSON &json);
JSON parse(const std::string &str);
void swap(JSON &first, JSON &second);

#endif