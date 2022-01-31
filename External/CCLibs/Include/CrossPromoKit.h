//
//  CrossPromoKit.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 15/06/15.
//
//

#ifndef CCLibs_CrossPromoKit_h
#define CCLibs_CrossPromoKit_h

#include "base/CCValue.h"


class CrossPromoKit {
public:
    class AppInfo: public std::enable_shared_from_this<AppInfo> {
    public:
        AppInfo(const cocos2d::ValueMap &info, const std::string &baseUrl);
        
        std::string getName() const;
        std::string getDesciption() const;

        std::string getUrlForResource(const std::string &name) const;
        void downloadResource(const std::string &name);
        bool isResourceDownloaded(const std::string &name) const;
        std::string getResourceLocalPath(const std::string &name) const;
        
        bool isInstalled() const;
        
        void markRewarded();
        
        void showInAppStore();

        bool isValid() const;
        
        std::string getEntryIdentifier() const;
        
    private:
        static const std::string KEY_NAME;
        static const std::string KEY_DESCRIPTION;
        static const std::string KEY_APP_ID;
        static const std::string KEY_URL_SCHEMES;
        static const std::string KEY_PROVIDER_TOKEN;
        static const std::string KEY_PACKAGE_NAME;
        static const std::string KEY_ENTRY_ID;
        static const std::string KEY_ANDROID_STORE_URL;
        
        friend class CrossPromoKit;
        bool wasRewarded() const;        
        
        std::vector<std::string> getRequiredFields() const; //for validation
        
        cocos2d::Value getValueForKey(const std::string &key) const {
            auto it = info.find(key);
            if (it != info.end()) {
                return (*it).second;
            }
            return cocos2d::Value::Null;
        }
        
        std::string getLocalizedValue(const std::string &key, const std::string &defaultValue) const;
        
    private:
        cocos2d::ValueMap info;
        std::string baseUrl;
    };
    
    using AppInfoPtr = std::shared_ptr<AppInfo>;
    
public:
    class Rewarder {
    public:
        virtual ~Rewarder() {}
        virtual void makeRewardsForInstallingCrossPromoApps(const std::vector<std::shared_ptr<AppInfo>> &apps) = 0;
    };
    
    static CrossPromoKit *getInstance() {
        static auto instance = new CrossPromoKit;
        return instance;
    }
    
    void startWithBaseUrl(const std::string &url, std::shared_ptr<Rewarder> rewarder);
    
    void makeRewards();

    std::vector<std::shared_ptr<AppInfo>> getNotInstalledApps() const;
    std::shared_ptr<AppInfo> getRandomNotInstalledApp() const;
    
    void showInAppStore(const std::shared_ptr<AppInfo> app);
    
private:
    std::string getInfoUrl() const;
    
    void loadCachedAppInfos();
    void downloadAppInfos();
    void parseData(const cocos2d::ValueVector &vec);
    std::string getFullPathForCacheFile() const;

private:
    CrossPromoKit();
    ~CrossPromoKit();
    
    std::string baseUrl;
    std::string affilateToken = "10lJDF";
    std::vector<std::shared_ptr<AppInfo>> apps;
    std::shared_ptr<Rewarder> rewarder;
};

#endif
