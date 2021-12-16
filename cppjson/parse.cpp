#include <array>
#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <cstring>

#include "cppjson.hpp"

using std::isdigit;
using std::logic_error;
using std::pair;
using std::string;

string::size_type skipWhitespaces(const string &input, string::size_type index);

pair<JSON, string::size_type> parseJSON(const string &input, string::size_type index);

pair<JSON, string::size_type> parseJSONArray(const string &input, string::size_type index);

pair<JSON, string::size_type> parseJSONNumber(const string &input, string::size_type index);

string::size_type parseKeyword(const string &input, string::size_type index, const string &keyword);

pair<std::array<char, 4>, string::size_type> parseEspacedChar(const string &input, string::size_type index);

std::pair<char16_t, string::size_type> parseUTF16EscapedValue(const string &input, string::size_type index);

class SyntaxError : logic_error
{
public:
    SyntaxError() : logic_error("JSON syntax error")
    {
    }
};

JSON parse(const string &str)
{
    return parseJSON(str, 0).first;
}

pair<JSON, string::size_type> parseJSON(const string &input, string::size_type index)
{
    auto ch = input[index];
    if (ch == '{')
        return parseJSONObject(input, index);
    if (ch == '[')
        return parseJSONArray(input, index);
    if (ch == '"')
        return parseJSONString(input, index);
    if (isdigit(ch) || ch == '-')
        return parseJSONNumber(input, index);
    if (ch == 't')
    {
        index = parseKeyword(input, index, "true");
        return std::make_pair(JSON(true), index);
    }
    if (ch == 'f')
    {
        index = parseKeyword(input, index, "false");
        return std::make_pair(JSON(false), index);
    }
    if (ch == 'n')
    {
        index = parseKeyword(input, index, "null");
        return std::make_pair(JSON(nullptr), index);
    }

    throw SyntaxError();
}

// The program looked ahead the next character first, and confirmed the what
// JSON value would be next, before it actually calls the function to parse a
// value, so the initial `index` value must points to the first character of
// a JSON value, hence we can simply skip it without checking.

pair<JSON, string::size_type> parseJSONObject(const string &input, string::size_type index)
{
    index++;
}

pair<JSON, string::size_type> parseJSONArray(const string &input, string::size_type index)
{
    index++;
}

pair<JSON, string::size_type> parseJSONString(const string &input, string::size_type index)
{
    string::size_type start = index;

    std::ostringstream oss;

    while (index != input.size())
    {
        if (input[index] == '\\')
        {
            if (index + 1 >= input.size())
                throw SyntaxError();

            oss << string(start, index);

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

                    if (v1.second + 1 < input.size() && input[v1.second] == '\\' && input[v1.second + 1])
                    {
                        auto v2 = parseUTF16EscapedValue(input, index);
                        if (isTrailSurrogate(v2.first))
                        {
                            auto lowSurrogate = v1.first;
                            auto highSurrogate = v2.first;
                            auto codePoint = calculateCodepoint(highSurrogate, lowSurrogate);
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
        }
        else if (input[index] == '"')
        {
            oss << string(start, index);
            index++;
            return std::make_pair(oss.str(), index);
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

    // For now we will just use string's stod function to parse the number.

    // JSON doesn't supports hex literals, but `strtod` can parse them.
    if (isHexLiteral(input, index))
        throw SyntaxError();

    const char *cStr = input.c_str();
    char *endPtr = const_cast<char *>(cStr);

    double result = std::strtod(cStr, &endPtr);

    if (cStr == endPtr)
        throw SyntaxError();
    else if (result == HUGE_VAL)
        throw std::range_error("The number is too large to parse as double.");

    return std::make_pair(JSON(result), index + (endPtr - cStr));
}

string::size_type parseKeyword(const string &input, string::size_type index, const string &keyword)
{
    auto kwIndex = 0;

    while (index != input.size() && kwIndex != keyword.size())
    {
        if (input[index] != keyword[kwIndex])
            break;

        index++;
        kwIndex++;
    }

    if (kwIndex != keyword.size())
        throw SyntaxError();

    return index;
}

string::size_type skipWhitespaces(const string &input, string::size_type index)
{
    while (index != input.size() && isspace(input[index]))
        index++;
    return index;
}

inline bool isHexLiteral(const string &input, string::size_type index)
{
    return index + 1 < input.size() && (input[index + 1] == 'x' || input[index + 1] == 'X');
}

std::pair<char16_t, string::size_type> parseUTF16EscapedValue(const string &input, string::size_type index)
{
    ensureExactlyHas4Digits(input, index + 2);
    char16_t v = strtoul(input.c_str(), nullptr, 16);
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
    return (static_cast<char32_t>(leadSurroatge - 0xD800u) << 10) + (trailSurrogate - 0xDC00u);
}

void writeAsUTF8CodeUnits(std::ostringstream &oss, char32_t codepoint)
{
    if (codepoint <= 0x00007F)
    {
        oss << codepoint;
    }
    else if (codepoint <= 0x0007FF)
    {
        oss << (0b11000000 | (codepoint >> 6));
        oss << (0b10000000 | (codepoint & 0b111111));
    }
    else if (codepoint <= 0x00D7FF || codepoint >= 0x00E000 && codepoint <= 0x00FFFF)
    {
        oss << (0b11100000 | (codepoint >> 12));
        oss << (0b10000000 | ((codepoint >> 6) & 0b111111);
        oss << (0b10000000 | (codepoint & 0b111111));
    }
    else if (codepoint <= 0x10FFFF)
    {
        oss << (0b11110000 | (codepoint >> 18));
        oss << (0b10000000 | (codepoint >> 12) & 0b111111);
        oss << (0b10000000 | (codepoint >> 6) & 0b111111);
        oss << (0b10000000 | (codepoint & 0b111111));
    }
    else
    {
        throw std::range_error("The codepoint is out of range.");
    }
}

void ensureExactlyHas4Digits(const string &input, string::size_type index)
{
    if (index + 4 < input.size() && isdigit(input[index + 4]))
        throw SyntaxError();

    for (auto i = index; i != index + 4; i++)
    {
        if (!isdigit(input[i]))
            throw SyntaxError();
    }
}