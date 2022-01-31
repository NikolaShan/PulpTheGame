//
// Created by Mike on 26.04.14.
//


#include "Utils/FilePath.h"
#include "Utils/StringUtils.h"
#include <vector>

std::string FilePath::appendPathComponent(const std::string &basePath, const std::string &pathComponent) {
    if (!basePath.length()) {
        return pathComponent;
    }
    if (basePath[basePath.length() - 1] != '/') {
        return basePath + "/" + pathComponent;
    }
    return basePath + pathComponent;
}

std::string FilePath::appendPathComponents(const std::string &basePath, const std::vector<std::string> &components) {
    std::string path = basePath;
    for (auto &&component: components) {
        if (!path.empty() && path[path.length() - 1] != '/') {
            path += "/";
        }
        path += component;
    }
    return path;
}

std::string FilePath::getExtension(const std::string &filePath) {
    std::string::size_type idx = filePath.rfind('.');
    if( (idx != std::string::npos) && (filePath.length() >= idx + 1) )  {
        return filePath.substr(idx + 1);
    }
    return "";
}

std::string FilePath::getLastPathComponent(const std::string &path) {
    auto components = StringUtils::split(path, "/");
    if (components.size()) {
        return components.back();
    }
    return "";
}

std::string FilePath::getFileName(const std::string &path) {
    std::string fileName = getLastPathComponent(path);
    auto dotIndex = fileName.rfind('.');
    if (dotIndex != std::string::npos) {
        return fileName.substr(0, dotIndex);
    }
    return fileName;
}

std::string FilePath::pathByRemovingLastPathComponent(const std::string &path) {
    auto components = StringUtils::split(path, "/");
    if (components.size() > 0) {
        components.pop_back();
    }
    return StringUtils::join(components, "/");
}