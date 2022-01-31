//
//  AppTurbo.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 06/03/16.
//
//

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <jni.h>

#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "CCLibs/Android/JniSupport.h"

#include "AppTurbo.h"
#include "Utils/AppUtils.h"

USING_NS_CC;

static const std::string APP_TURBO_CLASS = "org/cocos2dx/cpp/AppturboUnlockTools";

bool AppTurbo::isInstalled() {
    JniMethodInfo methodInfo;
    if (! JniHelper::getStaticMethodInfo(methodInfo, APP_TURBO_CLASS.c_str(), "isAppturboUnlockable", "()Z")) {
        CCASSERT(false, "ERROR: Can't find method for AppturboUnlockTools::isAppturboUnlockable");
        return false;
    }
    
    jboolean ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    return (bool)ret;
}

#endif