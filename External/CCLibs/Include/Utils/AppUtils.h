//
// Created by Edward on 15.06.15.
//



#ifndef __Utils_H_
#define __Utils_H_


#include <string>
#include <vector>
#include <sstream>
#include "math/CCGeometry.h"

namespace AppUtils {
    std::string getBundleIdentifier();
    
    bool isInstalledAppWithScheme(const std::string &scheme);
    
    std::string getPreferredLanguage();
    
    bool isTablet();
    bool isIPhoneX();
    
    bool isSlowDevice();
    
    cocos2d::Size getScreenSizeInDpi();
    
    std::string getVersionName();
    int getBuildNumber();
    
    bool backgroundMusicShouldBeSilenced();
    
    std::string getReferrer();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    bool requestSystemReview();
#endif
};


#endif //__Utils_H_
