//
// Created by Mike on 26.04.14.
//



#pragma once

#include <string>
#include <vector>

namespace FilePath {
    std::string getLastPathComponent(const std::string &path);
    std::string appendPathComponent(const std::string &path, const std::string &component);
    std::string appendPathComponents(const std::string &path, const std::vector<std::string> &component);
    std::string getExtension(const std::string &path);
    std::string getFileName(const std::string &path);
    std::string pathByRemovingLastPathComponent(const std::string &path);
};

