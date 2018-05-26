#pragma once

#include <string>

namespace proxybench {

class Strings
{
public:
    static std::string trim(std::string const& str)
    {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    static std::string replace(std::string const& str, char o, char n)
    {
        std::string result = str;
        size_t pos;
        while ((pos = result.find(o)) != std::string::npos)
            result[pos] = n;
        return result;
    }

public:
    Strings() = delete;
};

} // namespace proxybench {
