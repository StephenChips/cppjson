#include <array>
#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <cstring>
#include <limits>
#include <cerrno>

#include "cppjson.hpp"

using std::isdigit;
using std::logic_error;
using std::pair;
using std::string;

pair<JSON, string::size_type> parseJSON(const string &input, string::size_type index);
pair<JSON, string::size_type> parseJSONObject(const string &input, string::size_type index);
pair<JSON, string::size_type> parseJSONArray(const string &input, string::size_type index);
pair<std::string, string::size_type> parseStringLiteral(const string &input, string::size_type index);
pair<JSON, string::size_type> parseJSONNumber(const string &input, string::size_type index);
string::size_type expectString(const string &input, string::size_type index, const string &keyword);
string::size_type skipWhitespaces(const string &input, string::size_type index);
inline bool isHexLiteral(const string &input, string::size_type index);
inline bool isOctalLiteral(const string &input, string::size_type index);
std::pair<char16_t, string::size_type> parseUTF16EscapedValue(const string &input, string::size_type index);
inline bool isLeadSurrogate(char16_t v);
inline bool isTrailSurrogate(char16_t v);
char32_t calculateCodepoint(char16_t leadSurroatge, char16_t trailSurrogate);
void writeAsUTF8CodeUnits(std::ostringstream &oss, char32_t codepoint);
void writeHexValueToBuffer(char buf[], const string &input, string::size_type index);

JSON parse(const string &str)
{
    auto result = parseJSON(str, 0);
    auto end = skipWhitespaces(str, result.second);
    if (end != str.size())
        throw SyntaxError();
    return result.first;
}

pair<JSON, string::size_type> parseJSON(const string &input, string::size_type index)
{
    index = skipWhitespaces(input, index);
    auto ch = input[index];
    if (ch == '{')
        return parseJSONObject(input, index);
    else if (ch == '[')
        return parseJSONArray(input, index);
    else if (ch == '"')
    {
        auto result = parseStringLiteral(input, index);
        return std::make_pair(JSON(result.first), result.second);
    }
    else if (isdigit(ch) || ch == '-')
        return parseJSONNumber(input, index);
    else if (ch == 't')
    {
        index = expectString(input, index, "true");
        return std::make_pair(JSON(true), index);
    }
    else if (ch == 'f')
    {
        index = expectString(input, index, "false");
        return std::make_pair(JSON(false), index);
    }
    else if (ch == 'n')
    {
        index = expectString(input, index, "null");
        return std::make_pair(JSON(nullptr), index);
    }
    else
        throw SyntaxError();
}

// The program looked ahead the next character first, and confirmed the what
// JSON value would be next, before it actually calls the function to parse a
// value, so the initial `index` value must points to the first character of
// a JSON value, hence we can simply skip it without checking.

pair<JSON, string::size_type> parseJSONObject(const string &input, string::size_type index)
{
    std::map<std::string, JSON> result;
    index = skipWhitespaces(input, index + 1);

    if (index == input.size())
        throw SyntaxError();
    if (input[index] == '}')
        return std::make_pair(JSON(result), index + 1);

    while (true)
    {
        index = skipWhitespaces(input, index);
        auto objectKey = parseStringLiteral(input, index);
        index = skipWhitespaces(input, objectKey.second);
        index = expectString(input, index, ":");
        index = skipWhitespaces(input, index);
        auto objectValue = parseJSON(input, index);
        index = skipWhitespaces(input, objectValue.second);

        if (input[index] == '}')
        {
            result[objectKey.first] = objectValue.first;
            return std::make_pair(JSON(result), index + 1);
        }
        else if (input[index] == ',')
        {
            result[objectKey.first] = objectValue.first;
            index++;
        }
        else
            throw SyntaxError();
    }

    throw SyntaxError();
}

pair<JSON, string::size_type> parseJSONArray(const string &input, string::size_type index)
{
    std::vector<JSON> result;
    index = skipWhitespaces(input, index + 1);

    if (index == input.size())
        throw SyntaxError();
    if (input[index] == ']')
        return std::make_pair(JSON(result), index + 1);

    while (true)
    {
        auto jsonVal = parseJSON(input, index);
        index = skipWhitespaces(input, jsonVal.second);

        if (index == input.size())
            throw SyntaxError();
        else if (input[index] == ']')
        {
            result.push_back(jsonVal.first);
            return std::make_pair(JSON(result), index + 1);
        }
        else if (input[index] == ',')
        {
            result.push_back(jsonVal.first);
            index = skipWhitespaces(input, index + 1);
        }
        else
            throw SyntaxError();
    }
}

pair<std::string, string::size_type> parseStringLiteral(const string &input, string::size_type index)
{
    // Skip the double-quote at the begining
    index++;

    string::size_type start = index;
    std::ostringstream oss;

    while (index != input.size())
    {
        if (input[index] == '\\')
        {
            if (index + 1 >= input.size())
                throw SyntaxError();

            oss << input.substr(start, index - start);

            auto ch = input[index + 1];
            if (ch == 'u')
            {
                auto v1 = parseUTF16EscapedValue(input, index);
                if (isLeadSurrogate(v1.first))
                {
                    // If the escaped value we just read is a lead surrogate, there must be a trail surrogate right after it.
                    // Together they indicate a single Unicode character.
                    //
                    // Since the value we have just parsed is a lead surrogate, we need to get its correspondent trail surrogate.

                    if (v1.second + 1 < input.size() && input[v1.second] == '\\' && input[v1.second + 1] == 'u')
                    {
                        auto v2 = parseUTF16EscapedValue(input, v1.second);
                        if (isTrailSurrogate(v2.first))
                        {
                            auto leadSurrogate = v1.first;
                            auto trailSurrogate = v2.first;
                            auto codePoint = calculateCodepoint(leadSurrogate, trailSurrogate);
                            writeAsUTF8CodeUnits(oss, codePoint);
                        }
                        else
                        {
                            // The trail surrogate isn't valid because it's value is out of range (which should be between 0xDC00 to 0xDFFF).
                            // In such case, two values are treated as two codepoints individually and will be written to the output together.
                            writeAsUTF8CodeUnits(oss, v1.first);
                            writeAsUTF8CodeUnits(oss, v2.first);
                        }
                        index = v2.second;
                    }
                    else
                    {
                        // There are no another UTF-16 escaped value after the lead surrogate, which is invalid. The program will write the
                        // lead surrogate to the output then continue.
                        writeAsUTF8CodeUnits(oss, v1.first);
                        index = v1.second;
                    }
                }
                else
                {
                    // If the first escaped value isn't a lead surrogate, we will regard it as an Unicode codepoint and will write it to the
                    // output (It might be a trail surrogate but we need not to distinguish it).
                    writeAsUTF8CodeUnits(oss, v1.first);
                    index = v1.second;
                }
            }
            else
            {
                if (ch == '\\')
                    oss << '\\';
                else if (ch == '/')
                    oss << '/';
                else if (ch == 'b')
                    oss << '\b';
                else if (ch == 'f')
                    oss << '\f';
                else if (ch == 'n')
                    oss << '\n';
                else if (ch == 'r')
                    oss << '\r';
                else if (ch == 't')
                    oss << '\t';
                else
                    throw SyntaxError();

                index += 2;
            }

            start = index;
        }
        else if (input[index] == '"')
        {
            if (start < index)
            {
                oss << input.substr(start, index - start);
            }
            return std::make_pair(oss.str(), index + 1);
        }
        else
        {
            index++;
        }
    }

    throw SyntaxError();
}

pair<JSON, string::size_type> parseJSONNumber(const string &input, string::size_type index)
{
    // correspondent regex:
    // /^-?(0|([1-9][0-9]*))(\.[0-9]+)?((e|E)(-|\+)?[0-9]+)?$/

    // We could parse a number with our own codes, but for now we will just use `std::strtod`
    // instead.

    // JSON doesn't supports hex literals and octal literals, but `strtod` can parse them,
    // so we should exclude them before parsing.
    // Hex literal is something like 0x3, 0XE2, a literal started with 0x or 0X, and Octal
    // literals is somthing like 045, 022, a literal started with a leading zero.
    //
    // In JSON, if a number's interal part's first digit is zero, it must be followed by the
    // decimal point (.), no other digits can be appeared behind it.
    if (isHexLiteral(input, index) || isOctalLiteral(input, index))
        throw SyntaxError();

    const char *cStr = input.c_str();
    char *endPtr = const_cast<char *>(cStr);

    double result = std::strtod(cStr + index, &endPtr);

    if (endPtr == cStr)
        throw SyntaxError();
    else if (result == HUGE_VAL)
        result = std::numeric_limits<double>::infinity();
    else if (errno == ERANGE)
        // Underflow, like 1.0E-1000
        result = result > 0 ? 0 : -0;

    return std::make_pair(JSON(result), endPtr - cStr);
}

string::size_type expectString(const string &input, string::size_type index, const string &str)
{
    auto i = index;
    auto j = 0u;

    while (i != input.size() && j != str.size())
    {
        if (input[i] != str[j])
            break;

        i++, j++;
    }

    if (j != str.size())
        throw SyntaxError();

    return i;
}

string::size_type skipWhitespaces(const string &input, string::size_type index)
{
    while (index != input.size())
    {
        auto ch = input[index];
        if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
            index++;
        else
            break;
    }
    return index;
}

inline bool isHexLiteral(const string &input, string::size_type index)
{
    return index + 1 < input.size() && (input[index + 1] == 'x' || input[index + 1] == 'X');
}

inline bool isOctalLiteral(const string &input, string::size_type index)
{
    return index + 1 < input.size() && input[index] == '0' && isdigit(input[index + 1]);
}

std::pair<char16_t, string::size_type> parseUTF16EscapedValue(const string &input, string::size_type index)
{
    char digitBuffer[5];
    writeHexValueToBuffer(digitBuffer, input, index + 2);
    char16_t v = strtoul(digitBuffer, nullptr, 16);
    return std::make_pair(v, index + 6);
}

inline bool isLeadSurrogate(char16_t v)
{
    return v >= 0xD800u && v <= 0xDBFFu;
}

inline bool isTrailSurrogate(char16_t v)
{
    return v >= 0xDC00u && v <= 0xDFFFu;
}

char32_t calculateCodepoint(char16_t leadSurroatge, char16_t trailSurrogate)
{
    return (static_cast<char32_t>(leadSurroatge - 0xD800u) << 10) + (trailSurrogate - 0xDC00u) + 0x10000;
}

void writeAsUTF8CodeUnits(std::ostringstream &oss, char32_t codepoint)
{
    if (codepoint <= 0x00007F)
    {
        oss << static_cast<char>(codepoint);
    }
    else if (codepoint <= 0x0007FF)
    {
        oss << static_cast<char>(0b11000000 | (codepoint >> 6));
        oss << static_cast<char>(0b10000000 | (codepoint & 0b111111));
    }
    else if (codepoint <= 0x00FFFF)
    {
        // Strictly speaking, codepoints 0x00D800..0x00DFFF should only be used in UTF-16 text as
        // surrogate pair. This is how JSON escapes unicode that takes more than two bytes.
        // But in case we met an unpaired surrogate, instead throwing a syntax error, we will still
        // translate it to correspondent UTF-8 codeunits as if it was a normal unicode character
        // for this is how JSON.parse(...) behaves in browser.

        oss << static_cast<char>(0b11100000 | (codepoint >> 12));
        oss << static_cast<char>(0b10000000 | ((codepoint >> 6) & 0b111111));
        oss << static_cast<char>(0b10000000 | (codepoint & 0b111111));
    }
    else if (codepoint <= 0x10FFFF)
    {
        oss << static_cast<char>(0b11110000 | (codepoint >> 18));
        oss << static_cast<char>(0b10000000 | (codepoint >> 12) & 0b111111);
        oss << static_cast<char>(0b10000000 | (codepoint >> 6) & 0b111111);
        oss << static_cast<char>(0b10000000 | (codepoint & 0b111111));
    }
    else
    {
        throw std::range_error("The codepoint is out of range.");
    }
}

void writeHexValueToBuffer(char xdigits[], const string &input, string::size_type index)
{
    if (index + 4 >= input.size())
        throw SyntaxError();

    for (auto i = 0; i < 4; i++)
    {
        if (!std::isxdigit(input[index + i]))
            throw SyntaxError();
    }

    for (auto i = 0; i < 4; i++)
    {
        xdigits[i] = input[index + i];
    }

    xdigits[4] = '\0';
}