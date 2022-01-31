//
//  FileUrlCache.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 18/06/15.
//
//

#ifndef CCLibs_FileCache_h
#define CCLibs_FileCache_h

#include <set>
#include <vector>
#include <string>

class FileUrlCache {
public:
    struct CacheResult {
        bool success;
        std::string filePath;
        
        bool fileWasRemoved;
        
        static CacheResult successfullyDownloaded(const std::string &path) {
            CacheResult ret;
            ret.success = true;
            ret.filePath = path;
            ret.fileWasRemoved = false;
            return ret;
        }
        
        static CacheResult downloadFailure() {
            CacheResult ret;
            ret.success = false;
            ret.filePath = "";
            ret.fileWasRemoved = false;
            return ret;
        }
        
        static CacheResult fileRemoved() {
            CacheResult ret;
            ret.success = true;
            ret.filePath = "";
            ret.fileWasRemoved = true;
            return ret;
        }
    };
    
    using Completion = std::function<void(const CacheResult &result)>;
    
    static FileUrlCache *getInstance() {
        static auto instance = new FileUrlCache;
        return instance;
    }
    
    void cacheDataAtUrl(const std::string &url, Completion completion, time_t expirationTime = 86400);
    bool hasCachedDataForUrl(const std::string &url) const;
    std::string getFileCacheForUrl(const std::string &url) const;
    
private:
    void shouldDownloadResourceWithUrl(const std::string &url, time_t expirationTime, std::function<void(bool should, uint64_t newVersion, bool wasJustRemoved)> result);
    
    void performCacheDataAtUrl(const std::string &url, uint64_t fileVersion, Completion completion);
    void performCache(const char *data, size_t dataSize, const std::string &url, uint64_t fileVersion);
    void didFailCacheForUrl(const std::string &url);
    
    std::string getFileNameForUrl(const std::string &url) const;
    
    FileUrlCache() {}
    ~FileUrlCache() {}
    
private:
    using CompletionsVec = std::vector<Completion>;
    using CompletionsMap = std::map<std::string, CompletionsVec>;
    
    CompletionsMap activeRequests;
};

#endif
