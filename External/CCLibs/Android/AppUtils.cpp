
#include <jni.h>

#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "JniSupport.h"

#include "Utils/AppUtils.h"

USING_NS_CC;

static const std::string APP_UTILS_CLASS = "com/planemo/libs/support/AppUtils";

namespace AppUtils {
	std::string getBundleIdentifier() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "getPackageName", "()Ljava/lang/String;")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::getPackageName");
            return "";
        }
        jstring jident = (jstring) methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        const std::string ret = JniHelper::jstring2string(jident);
        methodInfo.env->DeleteLocalRef(jident);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }
    
    bool isInstalledAppWithScheme(const std::string &scheme) {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "isAppInstalled", "(Ljava/lang/String;)Z")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::isAppInstalled");
            return false;
        }
        jstring jscheme = methodInfo.env->NewStringUTF(scheme.c_str());
        jboolean ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, jscheme);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jscheme);
        return (bool)ret;
    }
    
    std::string getPreferredLanguage() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "getPreferredLanguage", "()Ljava/lang/String;")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::getPreferredLanguage");
            return "";
        }
        jstring jident = (jstring) methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        const std::string ret = JniHelper::jstring2string(jident);
        methodInfo.env->DeleteLocalRef(jident);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }

    bool isTablet() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "isTablet", "()Z")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::isTablet");
            return false;
        }
       
        jboolean ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return (bool)ret;
    }
    
    bool isSlowDevice() {
        //all android devices are slow AHAHAHAHHa!!!111!!
        return false;
    }

    cocos2d::Size getScreenSizeInDpi() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "getScreenSizeInDp", "()[I")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::getScreenSizeInDp");
            return  cocos2d::Size(0,0);
        }   
        jintArray jarray = (jintArray)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        std::vector<int> list = JniSupport::JavaIntArrayToVectorInt(jarray);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jarray);
        return cocos2d::Size(list.at(0), list.at(1));  
    }

    std::string getVersionName() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "getVersionName", "()Ljava/lang/String;")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::getVersionName");
            return "";
        }
        jstring jident = (jstring) methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        const std::string ret = JniHelper::jstring2string(jident);
        methodInfo.env->DeleteLocalRef(jident);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }

    int getBuildNumber() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "isTablet", "()I")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::getBuildNumber");
            return false;
        }
       
        jint ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return (int)ret;
    }

    bool backgroundMusicShouldBeSilenced() {
    	return false;
    }

    std::string getReferrer() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, APP_UTILS_CLASS.c_str(), "getReferrer", "()Ljava/lang/String;")) {
            CCASSERT(false, "ERROR: Can't find method for AppUtils::getReferrer");
            return "";
        }
        jstring jval = (jstring) methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        const std::string ret = JniHelper::jstring2string(jval);
        methodInfo.env->DeleteLocalRef(jval);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }
}













