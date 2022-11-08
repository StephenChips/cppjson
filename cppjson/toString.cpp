#include "cppjson.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <ios>

std::string toString(const JSON &json);
std::string escapeCharacters(const std::string &str);
std::string toStringArray(const JSON &json);

std::string toString(const JSON &json)
{
    switch (json.type())
    {
    case JSON::Bool:
        return json.getBool() ? "true" : "false";
    case JSON::String:
        return escapeCharacters(json.getString());
    case JSON::Null:
        return "null";
    case JSON::Number:
    {
        double val = json.getNumber();
        if (std::isnan(val) || std::isinf(val))
        {
            return "null";
        }
        else
        {
            std::ostringstream oss;
            oss << val;
            return oss.str();
        }
    }
    case JSON::Array:
    {
        auto s = std::string("[");
        auto &array = json.getArray();

        for (auto it = array.begin(); it != array.end(); it++)
        {
            s += toString(*it);
            if (it != array.end() - 1)
                s += ',';
        }

        s += ']';

        return s;
    }
    case JSON::Object:
    {
        auto s = std::string("{");
        auto &object = json.getObject();

        for (auto it = object.begin(); it != object.end(); it++)
        {
            s += '"';
            s += escapeCharacters(it->first);
            s += "\":\"";
            s+= toString(it->second);
            s += '"';
            if (it != std::prev(object.end()))
                s += ',';
        }
        s += '}';

        return s;
    }
    default:
        return "";
    }
}

std::string escapeCharacters(const std::string &str)
{
    std::ostringstream oss;
    for (auto it = str.cbegin(); it != str.cend(); it++)
    {
        switch (*it)
        {
        case '\n':
            oss << "\\n";
            break;
        case '\r':
            oss << "\\r";
            break;
        case '\f':
            oss << "\\f";
            break;
        case '\t':
            oss << "\\t";
            break;
        case '\b':
            oss << "\\b";
            break;
        default:
            // The char type's signedness depends on the compiler and the platform, in ARM and PowerPC it is unsigned
            // but it is signed in x86 and x64. We must cast it to unsigned char before comparing, otherwise if the
            // value is larger than 127, the result will probably be wrong (if it is a signed char).
            //
            // Here the value is implicitly casted to unsigned since 0x20u is a unsigned integer literal, according to
            // the specification, it one operand is unsigned, the other should be casted to unsigned too, before doing
            // the comparison.
            //
            // 0x20u is the smallest representable Unicode value, which is a space. Characters smaller than it are control
            // characters and should be escaped, this is browser's function `JSON.stringify(...)`;'s behaviour.
            if (*it < 0x20u)
            {
                oss << "\\u" << std::setfill('0') << std::setw(4) << std::hex << static_cast<unsigned long>(*it);
            }
            else
            {
                oss << *it;
            }
        }
    }

    return oss.str();
}