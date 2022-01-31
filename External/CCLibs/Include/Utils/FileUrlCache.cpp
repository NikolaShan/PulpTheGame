//
//  FileCache.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 18/06/15.
//
//

#include "network/HttpRequest.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"

#include "base/CCUserDefault.h"
#include "platform/CCFileUtils.h"

#include "Utils/FileUrlCache.h"
#include "Utils/FilePath.h"
#include "Utils/StringUtils.h"

USING_NS_CC;
using namespace network;

void FileUrlCache::cacheDataAtUrl(const std::string &url, Completion completion, time_t expirationTime) {
    shouldDownloadResourceWithUrl(url, expirationTime, [this, url, completion](bool should, uint64_t newVersion, bool wasJustRemoved) {
        if (should) {
            performCacheDataAtUrl(url, newVersion, completion);
        } else if (completion) {
            CacheResult result = {
                .success = wasJustRemoved || hasCachedDataForUrl(url),
                .filePath = wasJustRemoved ? "" : getFileCacheForUrl(url),
                .fileWasRemoved = wasJustRemoved
            };
            completion(result);
        }
    });
}

void FileUrlCache::shouldDownloadResourceWithUrl(const std::string &url, time_t expirationTime, std::function<void (bool, uint64_t, bool wasJustRemoved)> result) {
    const std::string fileName = getFileNameForUrl(url);
    
    auto ud = UserDefault::getInstance();
    const std::string versionStr = ud->getStringForKey(fileName.c_str(), "0");
    const uint64_t version = ::StringUtils::toNumber<uint64_t>(versionStr);

    // Don't check for resource updates too often
    const std::string lastTimeCheckedKey = "t" + fileName;
    const uint64_t currentTime = time(nullptr);
    if (version > 0 && hasCachedDataForUrl(url)) {
        const std::string lastTimeCheckedStr = ud->getStringForKey(lastTimeCheckedKey.c_str(), "0");
        const uint64_t lastTimeChecked = ::StringUtils::toNumber<uint64_t>(lastTimeCheckedStr);
        if (currentTime > lastTimeChecked && (currentTime - lastTimeChecked < expirationTime) ) {
            result(false, version, false);
            return;
        }
    }
    ud->setStringForKey(lastTimeCheckedKey.c_str(), ::StringUtils::toString(currentTime));

    if (url.find("api.planemostd.com") == std::string::npos) {
        result(true, currentTime, false);
        return;
    }
    
    //check is newer version available
    const std::string baseRequestUrl = "https://api.planemostd.com/common/file_modification_date.php";
    const std::string body = "file_url=" + url;
    
    auto request = new HttpRequest;
    request->setRequestType(HttpRequest::Type::GET);
    request->setUrl(baseRequestUrl.c_str());
    request->setRequestData(body.c_str(), body.length());
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback([this, result, version, url, fileName](HttpClient *client, HttpResponse *response) {
        if (response->isSucceed() && response->getResponseData()
            && response->getResponseCode() < 300 && response->getResponseCode() >= 200) {
            auto data = response->getResponseData();
            const std::string newVersionStr(data->begin(), data->end());
            const uint64_t newVersion = ::StringUtils::toNumber<uint64_t>(newVersionStr);
            
            const std::string cacheFilePath = getFileCacheForUrl(url);
            if (newVersion == 0 && FileUtils::getInstance()->isFileExist(cacheFilePath)) {
                FileUtils::getInstance()->removeFile(cacheFilePath);
                UserDefault::getInstance()->setStringForKey(fileName.c_str(), "0");
                result(false, 0, true);
            } else {
                result(newVersion > version, newVersion, false);
            }
            
        } else {
            result(true, version, false);
        }
    });
    
    HttpClient::getInstance()->send(request);
    request->release();
}

void FileUrlCache::performCacheDataAtUrl(const std::string &url, uint64_t fileVersion, Completion completion) {
    //check if we already downloading resource with the same url
    auto it = activeRequests.find(url);
    if (it == activeRequests.end()) {
        activeRequests[url] = { completion };
    } else {
        (*it).second.push_back(completion);
        return;
    }
    
    auto request = new HttpRequest;
    request->setRequestType(HttpRequest::Type::GET);
    request->setUrl(url.c_str());
    request->setResponseCallback([this, url, fileVersion](HttpClient *client, HttpResponse *response) {
        if (response->isSucceed() && response->getResponseData()
            && response->getResponseCode() < 300 && response->getResponseCode() >= 200) {
            auto data = response->getResponseData();
            this->performCache(&((*data)[0]), data->size(), url, fileVersion);
            
        } else if (response->getErrorBuffer()) {
            cocos2d::log("Warning: Failed to cache file at url: %s with code %ld and error: %s", url.c_str(),
                         response->getResponseCode(), response->getErrorBuffer());
            this->didFailCacheForUrl(url);
            
        } else {
            cocos2d::log("Warning: Failed to cache file at url: %s with response code %ld",
                         url.c_str(), response->getResponseCode());
            this->didFailCacheForUrl(url);
        }
    });
    
    HttpClient::getInstance()->send(request);
    request->release();
}

void FileUrlCache::performCache(const char *data, size_t dataSize, const std::string &url, uint64_t fileVersion) {
    const std::string filePath = getFileCacheForUrl(url);
    
    FILE *fp = fopen(filePath.c_str(), "w");
  
    if (fp) {
        fwrite(data, sizeof(char), dataSize, fp);
        fclose(fp);
        
        if (fileVersion > 0) {
            const std::string fileName = getFileNameForUrl(url);
            UserDefault::getInstance()->setStringForKey(fileName.c_str(), ::StringUtils::toString(fileVersion));
        }
        
        auto it = activeRequests.find(url);
        if (it != activeRequests.end()) {
            CompletionsVec completions = (*it).second;
            activeRequests.erase(it);
            for (Completion completion: completions) {
                if (completion) {
                    completion(CacheResult::successfullyDownloaded(filePath));
                }
            }
        }
        
    } else {
        didFailCacheForUrl(url);
    }
}

void FileUrlCache::didFailCacheForUrl(const std::string &url) {
    auto it = activeRequests.find(url);
    if (it != activeRequests.end()) {
        CompletionsVec completions = (*it).second;
        activeRequests.erase(it);
        for (Completion completion: completions) {
            if (completion) {
                completion(CacheResult::downloadFailure());
            }
        }
    }
}

bool FileUrlCache::hasCachedDataForUrl(const std::string &url) const {
    return FileUtils::getInstance()->isFileExist(getFileCacheForUrl(url));
}

std::string FileUrlCache::getFileNameForUrl(const std::string &url) const {
    static const uint64_t A = 54059;
    static const uint64_t B = 76963;
    
    uint64_t h = 31;
    for (auto c: url) {
        h = (h * A) ^ (c * B);
    }
    
    std::string fileName = ::StringUtils::toString(h);
    
    const std::string extension = FilePath::getExtension(url);
    if (!extension.empty()) {
        fileName += "." + extension;
    }
    return fileName;
}

std::string FileUrlCache::getFileCacheForUrl(const std::string &url) const {
    const std::string fileName = getFileNameForUrl(url);
    const std::string writablePath = FileUtils::getInstance()->getWritablePath();
    const std::string cachesPath = FilePath::appendPathComponent(writablePath, "AppCache");
    FileUtils::getInstance()->createDirectory(cachesPath);
    return FilePath::appendPathComponent(cachesPath, fileName);
}
