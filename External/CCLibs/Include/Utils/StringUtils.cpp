//
// Created by Mike on 25.04.14.
//

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "Utils/StringUtils.h"
#include "Utils/PrefexOutputIterator.h"


// trim from start
std::string &StringUtils::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &StringUtils::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &StringUtils::trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::string StringUtils::trimConst(const std::string &s) {
    auto ret = s;
    return trim(ret);
}

std::vector<std::string> StringUtils::split(const std::string &string, const std::string &separator) {
    std::vector<std::string> components;

    const size_t lastIndex = string.length();
    size_t nend = 0;
    size_t nbegin = 0;
    while (nend != -1) {
        nend = string.find(separator, nbegin);
        if (nend == -1) {
            if (nbegin < lastIndex) {
                components.push_back(string.substr(nbegin, string.length()-nbegin));
            }
        } else {
            components.push_back(string.substr(nbegin, nend-nbegin));
        }

        nbegin = nend + separator.size();
    }
    return components;
}

std::string &StringUtils::replaceAll(std::string &str, const std::string &from, const std::string &to) {
    if(from.empty())
        return str;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    return str;
}

std::string StringUtils::formatString(const std::string &source, const std::vector<std::string> &args) {
    std::string ret = source;
    for (int argNum = 0; argNum < args.size(); ++argNum) {
        const std::string arg = args[argNum];
        const std::string argHolder = "%" + toString(argNum + 1);
        const size_t loc = ret.find(argHolder);
        if (loc != std::string::npos) {
            ret.replace(loc, argHolder.size(), arg);
        }
    }
    return ret;
}

std::string StringUtils::join(const std::vector<std::string> &strings, const std::string &delimeter) {
    std::ostringstream ret;
    std::copy(strings.begin(), strings.end(), PrefexOutputIterator<std::string>(ret, delimeter));
    return ret.str();
}

bool StringUtils::startsWith(const std::string &str, const std::string &prefix) {
    if (str.size() && prefix.size() && str.size() >= prefix.size()) {
        return str.substr(0, prefix.size()) == prefix;
    }
    return false;
}

bool StringUtils::isNumber(const std::string &s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

std::string StringUtils::toLowerCase(const std::string &source) {
    std::string ret;
    ret.resize(source.size());
    std::transform(source.begin(), source.end(), ret.begin(), ::tolower);
    return ret;
}

std::string StringUtils::toUpperCase(const std::string &source) {
    std::string ret;
    ret.resize(source.size());
    std::transform(source.begin(), source.end(), ret.begin(), ::toupper);
    return ret;
}
