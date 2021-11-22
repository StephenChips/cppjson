#include <string>

#include "cppjson.hpp"

/**
 * @brief The default constructor will construct an empty JSON object.
 * 
 */
JSON::JSON() : valType(OBJECT) {}

/**
 * @brief  Construct a new JSON::JSON object holds a boolean.
 * 
 * @param val 
 */
JSON::JSON(bool val) : valType(BOOL), valBoolean(val) {}

/**
 * @brief Construct a new JSON::JSON object holds a number.
 * 
 * @param val 
 */
JSON::JSON(double val) : valType(NUMBER), valDouble(val){};

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
JSON::JSON(nullptr_t val) : valType(JSONNULL){};

/**
 * @brief Copy constructor
 * 
 * @param val 
 */
JSON::JSON(const JSON &val){};

/* A series of methods return the type of a JSON::JSON object. */

bool JSON::isBool() { return false; };
bool JSON::isNumber() { return false; };
bool JSON::isString() { return false; };
bool JSON::isNull() { return false; };
bool JSON::isObject() { return false; };
bool JSON::isArray() { return false; };

/* Entry-access methods, which are for JSON::JSON objects that represents JSON objects or arrays. */

JSON &JSON::operator[](const std::string &s){};
JSON &JSON::operator[](size_t idx){};

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