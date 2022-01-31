//
// Created by Mike on 25.04.14.
//



#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace StringUtils {
    // trim from start
    std::string &ltrim(std::string &s);

    // trim from end
    std::string &rtrim(std::string &s);

    // trim from both ends
    std::string &trim(std::string &s);

    std::string trimConst(const std::string &s);

    std::vector<std::string> split(const std::string &str, const std::string &separator);

    template<typename T>
    std::string toString(T value) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        std::ostringstream ss;
        ss << value;
        return ss.str();
#else
        return std::to_string(value);
#endif
    }

    template <typename T>
    T toNumber(const std::string &str) {
        T ret;
        std::istringstream(str) >> ret;
        return ret;
    }

    std::string &replaceAll(std::string &str, const std::string &from, const std::string &to);
    
    std::string toUpperCase(const std::string &str);
    std::string toLowerCase(const std::string &str);
    std::string capitalizeFirstLetter(const std::string &str);
    
    std::string formatString(const std::string &source, const std::vector<std::string> &args);
    
    std::string join(const std::vector<std::string> &strings, const std::string &delimeter);
    
    bool startsWith(const std::string &str, const std::string &prefix);
    
    bool isNumber(const std::string &s);
};

