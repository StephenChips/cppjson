#ifndef CPP_JSON
#define CPP_JSON

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

class JSON
{
public:
    enum Type
    {
        BOOL,
        NUMBER,
        STRING,
        JSONNULL,
        OBJECT,
        ARRAY,
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
    JSON(nullptr_t val);
    JSON(bool val);
    JSON(double val);
    JSON(std::string val);
    JSON(const JSON &val);
    JSON(JSON &&rhs);

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

    std::string &getString();
    const std::string &getString() const;

    std::vector<JSON> &getArray();
    const std::vector<JSON> &getArray() const;

    std::map<std::string, JSON> &getObject();
    const std::map<std::string, JSON> &getObject() const;

    size_t size() const;

    static JSON array();
    static JSON array(size_t sz);

    friend std::string toString(JSON &json);
    friend JSON parse(const std::string &str);
    friend void swap(JSON &first, JSON &second);
};

std::string toString(JSON &json);
JSON parse(const std::string &str);
void swap(JSON &first, JSON &second);

#endif