//
//  Localization.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 20/06/15.
//
//

#ifndef CCLibs_Localization_h
#define CCLibs_Localization_h

#include <string>
#include <memory>
#include <vector>
#include "format.h"
#include "base/CCValue.h"

class Localization {
public:
    static std::shared_ptr<Localization> getInstance() {
        static auto instance = std::make_shared<Localization>();
        return instance;
    }
    Localization(const std::string &language);
    Localization();
    ~Localization();
    
    //must be called before getting any instance of Localization
    static void setAvailableLanguages(const std::vector<std::string> &languages) {
        sAvailableLanguages = languages;
    }
    
    static void setSourceFileNames(const std::vector<std::string> &names) {
        sSourceFileNames = names;
    }
    
    static std::vector<std::string> getAvailableLanguages() {
        return sAvailableLanguages;
    }
    
    std::string getLanguage() const {
        return language;
    }
    void setLanguage(const std::string &language);
    std::string getString(const std::string &keyPath, const std::string &fallbackString = "") const;

    std::string getFormattedString(const std::string keyPath, fmt::ArgList args);
    FMT_VARIADIC(std::string, getFormattedString, const std::string);
    
    std::string getFormattedStringF(const std::string keyPath, const std::string fallbackFormat, fmt::ArgList args);
    FMT_VARIADIC(std::string, getFormattedStringF, const std::string, const std::string);

private:
    void loadLocalizationData();
    
private:
    static std::vector<std::string> sAvailableLanguages;
    static std::vector<std::string> sSourceFileNames;
    std::string language;
    cocos2d::ValueMap localizationData;
};

#define LocalizedString(_KEY_) Localization::getInstance()->getString(_KEY_)
#define FormattedLocalizedString(_KEY_, ...) Localization::getInstance()->getFormattedString(_KEY_, ##__VA_ARGS__)

#endif
