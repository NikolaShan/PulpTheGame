//
//  Localization.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 20/06/15.
//
//

#include "Utils/AppUtils.h"
#include "Utils/Collections.h"

#include "platform/CCFileUtils.h"

#include "Localization.h"

std::vector<std::string> Localization::sAvailableLanguages;
std::vector<std::string> Localization::sSourceFileNames;

Localization::Localization(const std::string &language) {
    setLanguage(language);
}

Localization::Localization() {
    setLanguage(AppUtils::getPreferredLanguage());
}

Localization::~Localization() {
    
}

void Localization::setLanguage(const std::string &language) {
    auto availableLanguages = getAvailableLanguages();
    bool setup = false;
    for (auto &&availableLanguage: availableLanguages) {
        if (language.find(availableLanguage) != std::string::npos) {
            this->language = availableLanguage;
            setup = true;
            break;
        }
    }
    if (setup) {
        //nothing to do
    } else if (language != "en") {
        setLanguage("en"); //try to set en be default
        return;
    } else if (!availableLanguages.empty()) {
        this->language = availableLanguages.front();
    } else {
        cocos2d::log("WARNING: no localization available.");
        return;
    }
    
    loadLocalizationData();
}

void Localization::loadLocalizationData() {
    //load all Localization/language/strings.plist
    if (sSourceFileNames.empty()) {
        sSourceFileNames.push_back("strings.plist");
    }
    localizationData.clear();
    for (const auto &fileName: sSourceFileNames) {
        const std::string stringsPath = "Localization/" + language + "/" + fileName;
        const cocos2d::ValueMap fileData = cocos2d::FileUtils::getInstance()->getValueMapFromFile(stringsPath);
        for (auto entry: fileData) {
            localizationData[entry.first] = entry.second;
        }
    }
}

std::string Localization::getString(const std::string &keyPath, const std::string &fallbackString) const {
    auto &value = Collections::getValueForKeyPath(localizationData, keyPath);
    switch (value.getType()) {
        case cocos2d::Value::Type::INT_KEY_MAP:
        case cocos2d::Value::Type::VECTOR:
        case cocos2d::Value::Type::NONE:
        case cocos2d::Value::Type::MAP:
            cocos2d::log("WARNING: Localization (%s) - value for keyPath %s not convertible to string or doesn't exist", language.c_str(), keyPath.c_str());
            break;
            
        default:
            return value.asString();
    }
    return fallbackString;
}

std::string Localization::getFormattedString(const std::string keyPath, fmt::ArgList args) {
    auto formatStr = getString(keyPath);
    auto result = fmt::format(formatStr, args);
    return result;
}

std::string Localization::getFormattedStringF(const std::string keyPath, const std::string fallbackFormat, fmt::ArgList args) {
    auto formatStr = getString(keyPath, fallbackFormat);
    auto result = fmt::format(formatStr, args);
    return result;
}
