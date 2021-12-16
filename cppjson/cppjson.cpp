#include <string>
#include <cassert>
#include <stdexcept>
#include <memory>

#include "cppjson.hpp"

/**
 * @brief The default constructor will construct an empty JSON object.
 * 
 */
JSON::JSON() : _type(OBJECT) {}

/**
 * @brief  Construct a new JSON::JSON object holds a boolean.
 * 
 * @param val 
 */
JSON::JSON(bool val) : _type(BOOL), valBoolean(val) {}

/**
 * @brief Construct a new JSON::JSON object holds a number.
 * 
 * @param val 
 */
JSON::JSON(double val) : _type(NUMBER), valNumber(val){};

/**
 * @brief Construct a new JSON::JSON object holds a string.
 * 
 * @param val 
 */
JSON::JSON(std::string val) : _type(STRING), valString(val){};

/**
 * @brief Construct a new JSON::JSON object holds a null value.
 * 
 * @param val 
 */
JSON::JSON(nullptr_t val) : _type(JSONNULL){};

/**
 * @brief Copy constructor
 * 
 * @param val 
 */
JSON::JSON(const JSON &rhs)
{
    if (!rhs.isObject() || isObject())
        valObject.empty();
    if (!rhs.isArray() || isArray())
        valArray.empty();

    switch (rhs._type)
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
        break;
    }

    _type = rhs._type;
}

JSON::JSON(JSON &&rhs) noexcept : JSON()
{
    swap(*this, rhs);
}

/* A series of methods return the _type of a JSON::JSON object. */

bool JSON::isBoolean() const { return _type == BOOL; };
bool JSON::isNumber() const { return _type == NUMBER; };
bool JSON::isString() const { return _type == STRING; };
bool JSON::isNull() const { return _type == JSONNULL; };
bool JSON::isObject() const { return _type == OBJECT; };
bool JSON::isArray() const { return _type == ARRAY; };

JSON::Type JSON::type() const { return _type; }

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
JSON &JSON::operator=(JSON rhs)
{
    if (setParentNodeFn)
        setParentNodeFn();

    swap(*this, rhs);
    return *this;
};

// Methods that gets the wrapping value under a JSON::JSON object.
// They should only work when the underlying value matches the returnning _type.

double &JSON::getNumber()
{
    if (_type == NUMBER)
        return valNumber;
    throw std::logic_error("The type is not number");
};

const double &JSON::getNumber() const
{
    return const_cast<const double &>(const_cast<JSON &>(*this).getNumber());
}

bool &JSON::getBool()
{
    if (_type == BOOL)
        return valBoolean;
    throw std::logic_error("The type is not boolean");
};

const bool &JSON::getBool() const
{
    return const_cast<const bool &>(const_cast<JSON &>(*this).getBool());
}

std::string &JSON::getString()
{
    if (_type == STRING)
        return valString;
    throw std::logic_error("The type is not string");
};

const std::string &JSON::getString() const
{
    return const_cast<const std::string &>(const_cast<JSON &>(*this).getString());
}

std::vector<JSON> &JSON::getArray()
{
    if (_type == ARRAY)
        return valArray;
    throw std::logic_error("The type is not array");
}

const std::vector<JSON> &JSON::getArray() const
{
    return const_cast<const std::vector<JSON> &>(const_cast<JSON &>(*this).getArray());
}

std::map<std::string, JSON> &JSON::getObject()
{
    if (_type == OBJECT)
        return valObject;
    throw std::logic_error("The type is not object");
}

const std::map<std::string, JSON> &JSON::getObject() const
{
    return const_cast<const std::map<std::string, JSON> &>(const_cast<JSON &>(*this).getObject());
}

/**
 * @brief Get the size of an JSON::JSON array or an object. returns -1 if the object isn't.
 * 
 * @return size_t 
 */
size_t JSON::size() const
{
    if (_type == OBJECT)
        return valObject.size();
    else if (_type == ARRAY)
        return valArray.size();
    else
        return -1;
};

/* A static method create an empty JSON::JSON array */
JSON JSON::array()
{
    JSON ret;
    ret._type = ARRAY;
    return ret;
};

JSON JSON::array(size_t sz)
{
    auto ret = array();
    ret.valArray = std::vector<JSON>(sz, nullptr);
    return ret;
};

void swap(JSON &first, JSON &second)
{
    using std::swap;

    swap(first._type, second._type);
    swap(first.valString, second.valString);
    swap(first.valNumber, second.valNumber);
    swap(first.valBoolean, second.valBoolean);
    swap(first.valArray, second.valArray);
    swap(first.valObject, second.valObject);
    swap(first.setParentNodeFn, second.setParentNodeFn);
    swap(first.absenceNode, second.absenceNode);
}