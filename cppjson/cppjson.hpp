#ifndef CPP_JSON
#define CPP_JSON

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

class JSON
{
private:
    struct Pos
    {
        const std::string *str;
        size_t start, end;

        Pos() = default;

        Pos(const std::string &str, size_t start, size_t end) : str(&str), start(start), end(end) {}
    };

    std::string valString;
    double valNumber;
    bool valBoolean;
    std::vector<JSON> valArray;
    std::map<std::string, JSON> valObject;
    struct Pos valPosition;

    std::unique_ptr<JSON> absenceNode;
    std::function<void()> setParentNodeFn;

    JSON(std::function<void()> setParentCallback): setParentNodeFn(setParentCallback) {};
public:
    enum Type
    {
        BOOL,
        NUMBER,
        STRING,
        JSONNULL,
        OBJECT,
        ARRAY,
        UNPARSED,
    } type;

    JSON();
    JSON(nullptr_t val);
    JSON(bool val);
    JSON(double val);
    JSON(std::string val);
    JSON(const JSON &val);
    JSON(const std::string &str, size_t start, size_t end);

    bool isBoolean();
    bool isNumber();
    bool isString();
    bool isNull();
    bool isObject();
    bool isArray();

    Type getType();

    JSON &operator[](const std::string &s);
    JSON &operator[](size_t idx);

    JSON &operator=(const JSON &val);

    bool getBool();
    double getNumber();
    std::string getString();
    std::vector<JSON> getArray();
    std::map<std::string, JSON> getObject();

    bool isNull();

    size_t size();

    void push_back(const JSON &val);
    void push_front(const JSON &val);
    void erase(size_t idx);
    void erase(size_t start, size_t end);
    void erase(const std::string &s);

    static JSON array();
    static JSON array(size_t sz);
};

std::string toString(JSON &json);
JSON parse(const std::string &str);


#endif