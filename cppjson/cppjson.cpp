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
JSON::JSON(double val) : type(NUMBER), valDouble(val){};

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

JSON::JSON(const JSON *parentNode) : type(ABSENCE), ptrParentNode(ptrParentNode) {};

JSON::JSON(const std::string &str, size_t start, size_t end) : type(UNPARSED), valPosition(str, start, end){};

/* A series of methods return the type of a JSON::JSON object. */

bool JSON::isBool() { return type == BOOL; };
bool JSON::isNumber() { return type == NUMBER; };
bool JSON::isString() { return type == STRING; };
bool JSON::isNull() { return type == JSONNULL; };
bool JSON::isObject() { return type == OBJECT; };
bool JSON::isArray() { return type == ARRAY; };
bool JSON::isAbsence() { return type == ABSENCE; };

/* Entry-access methods for JSON::JSON objects represents JSON objects or arrays. */

JSON &JSON::operator[](const std::string &s)
{
    if (isObject())
    {
        auto iter = valObject.find(s);
        if (iter == valObject.end()) {
            if (!absenceNode) {
                absenceNode = std::unique_ptr<JSON>(new JSON(*this));
            }
            return *absenceNode;
        }
        else {
            return iter->second;
        }
    }
    else if (isArray())
    {
        throw std::logic_error("JSON array can only use operator[] with positive integer argument.");
    }
    else
    {
        throw std::logic_error("Only JSON objects and arrays can use operator[].");
    }
};
JSON &JSON::operator[](size_t idx)
{
    assert(isObject() || isArray());
};

/**
 * @brief Assignment operator, updates a JSON::JSON object's value.
 * 
 * @param val 
 * @return JSON& 
 */
JSON &JSON::operator=(const JSON &val){};

// Methods that gets the wrapping value under a JSON::JSON object.
// They should only work when the underlying value matches the returnning type.

template <>
double JSON::get<double>(){};

template <>
bool JSON::get<bool>(){};

template <>
std::string JSON::get<std::string>(){};

template <>
nullptr_t JSON::get<nullptr_t>(){};

/**
 * @brief Get the size of an JSON::JSON array or an object. returns -1 if the object isn't.
 * 
 * @return size_t 
 */
size_t JSON::size(){};

/* Methods for adding and removing an array or object's entries. */
void JSON::push_back(const JSON &val){};
void JSON::push_front(const JSON &val){};
void JSON::erase(size_t idx){};
void JSON::erase(size_t start, size_t end){};
void JSON::erase(const std::string &s){};

/* A static method create an empty JSON::JSON array */
JSON JSON::array(){};
