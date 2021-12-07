#include <string>
#include <cassert>
#include <stdexcept>
#include <memory>

#include "cppjson.hpp"

/**
 * @brief The default constructor will construct an empty JSON object.
 * 
 */
JSON::JSON() : type(OBJECT) {}

/**
 * @brief  Construct a new JSON::JSON object holds a boolean.
 * 
 * @param val 
 */
JSON::JSON(bool val) : type(BOOL), valBoolean(val) {}

/**
 * @brief Construct a new JSON::JSON object holds a number.
 * 
 * @param val 
 */
JSON::JSON(double val) : type(NUMBER), valNumber(val){};

/**
 * @brief Construct a new JSON::JSON object holds a string.
 * 
 * @param val 
 */
JSON::JSON(std::string val){};

/**
 * @brief Construct a new JSON::JSON object holds a null value.
 * 
 * @param val 
 */
JSON::JSON(nullptr_t val) : type(JSONNULL){};

/**
 * @brief Copy constructor
 * 
 * @param val 
 */
JSON::JSON(const JSON &val){};

JSON::JSON(const std::string &str, size_t start, size_t end) : type(UNPARSED), valPosition(str, start, end){};

/* A series of methods return the type of a JSON::JSON object. */

bool JSON::isBoolean() { return type == BOOL; };
bool JSON::isNumber() { return type == NUMBER; };
bool JSON::isString() { return type == STRING; };
bool JSON::isNull() { return type == JSONNULL; };
bool JSON::isObject() { return type == OBJECT; };
bool JSON::isArray() { return type == ARRAY; };

JSON::Type JSON::getType() { return type; }

/* Entry-access methods for JSON::JSON objects represents JSON objects or arrays. */

JSON &JSON::operator[](const std::string &s)
{
    if (isObject())
    {
        auto iter = valObject.find(s);
        if (iter == valObject.end())
        {
            if (!absenceNode)
            {
                std::function<void()> setParentNode = [this, s]()
                {
                    this->valObject[s] = *this->absenceNode;
                    this->absenceNode.reset();
                };
                absenceNode = std::unique_ptr<JSON>(new JSON(setParentNode));
            }
            return *absenceNode;
        }
        else
        {
            return iter->second;
        }
    }
    else if (isArray())
    {
        throw std::logic_error("JSON array can only use operator[] with a positive integer argument.");
    }
    else
    {
        throw std::logic_error("Only JSON objects and arrays can use operator[].");
    }
};
JSON &JSON::operator[](size_t idx)
{
    if (isArray())
    {
        if (idx < 0 || idx >= valArray.size())
        {
            throw std::out_of_range("input index is out of JSON array's range");
        }
        return valArray[idx];
    }
    else if (isObject())
    {
        throw std::logic_error("JSON object can only use operator[] with a string argument.");
    }
    else
    {
        throw std::logic_error("Only JSON objects and arrays can use operator[]");
    }
};

/**
 * @brief Assignment operator, updates a JSON::JSON object's value.
 * 
 * @param val 
 * @return JSON& 
 */
JSON &JSON::operator=(const JSON &rhs)
{
    if (isObject())
        valObject.empty();
    if (isArray())
        valArray.empty();

    switch (rhs.type)
    {
    case OBJECT:
        valObject = rhs.valObject;
        break;
    case ARRAY:
        valArray = rhs.valArray;
        break;
    case STRING:
        valString = rhs.valString;
        break;
    case BOOL:
        valBoolean = rhs.valBoolean;
        break;
    case NUMBER:
        valNumber = rhs.valNumber;
    }

    if (setParentNodeFn)
        setParentNodeFn();

    type = rhs.type;

    return *this;
};

// Methods that gets the wrapping value under a JSON::JSON object.
// They should only work when the underlying value matches the returnning type.

double &JSON::getNumber()
{
    if (type == NUMBER)
        return valNumber;
    else
        throw std::logic_error("The type is not number");
};

bool &JSON::getBool()
{
    if (type == BOOL)
        return valBoolean;
    else
        throw std::logic_error("The type is not boolean");
};

std::string &JSON::getString()
{
    if (type == STRING)
        return valString;
    else
        throw std::logic_error("The type is not string");
};

std::vector<JSON> &JSON::getArray()
{
    if (type == ARRAY)
        return valArray;
    else
        throw std::logic_error("The type is not array");
}

std::map<std::string, JSON> &JSON::getObject()
{
    if (type == OBJECT)
        return valObject;
    else
        throw std::logic_error("The type is not object");
}

/**
 * @brief Get the size of an JSON::JSON array or an object. returns -1 if the object isn't.
 * 
 * @return size_t 
 */
size_t JSON::size()
{
    if (type == OBJECT)
        return valObject.size();
    else if (type == ARRAY)
        return valArray.size();
    else
        return -1;
};

/* A static method create an empty JSON::JSON array */
JSON JSON::array()
{
    auto ret = JSON();
    ret.type = ARRAY;
    return ret;
};
JSON JSON::array(size_t sz)
{
    auto ret = array();
    ret.valArray = std::vector<JSON>(sz, nullptr);
    return ret;
};
