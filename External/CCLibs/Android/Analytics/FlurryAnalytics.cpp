#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "FlurryAnalytics.h"
#include "../JniSupport.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/analytics/FlurryAnalytics";


class AndroidFlurryAnalytics: public FlurryAnalytics {
public:
    virtual void startSession(const std::string &apiKey) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "init", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::startSession");
        }
        jstring jappKey = env->NewStringUTF(apiKey.c_str());
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jappKey);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jappKey);
    }
    
    virtual void logPageView(const std::string &pageName) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "logPageView", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::logPageView");
        }
        jstring jpageName = env->NewStringUTF(pageName.c_str());
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jpageName);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jpageName);
    }
    
    virtual void logEvent(const std::string &event) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "logEvent", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::logEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jevent);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jevent);
    }
    
    virtual void logEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;Ljava/util/Map;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "logEvent", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::logEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        jobject jmap = JniSupport::StdStringMapToJavaMap(parameters);
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jevent, jmap);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jevent);
        env->DeleteLocalRef(jmap);
    }
    
    virtual void logTimedEvent(const std::string &event) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "logTimedEvent", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::logTimedEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jevent);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jevent);        
    }
    
    virtual void logTimedEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;Ljava/util/Map;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "logTimedEvent", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::logTimedEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        jobject jmap = JniSupport::StdStringMapToJavaMap(parameters);
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jevent, jmap);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jevent);
        env->DeleteLocalRef(jmap);
    }
    
    virtual void endTimedEvent(const std::string &event) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "endTimedEvent", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::endTimedEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jevent);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jevent);        
    }
    
    virtual void endTimedEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;Ljava/util/Map;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "endTimedEvent", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FlurryAnalytics::endTimedEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        jobject jmap = JniSupport::StdStringMapToJavaMap(parameters);
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jevent, jmap);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jevent);
        env->DeleteLocalRef(jmap);
    }
};

std::shared_ptr<FlurryAnalytics> FlurryAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<FlurryAnalytics>(std::make_shared<AndroidFlurryAnalytics>());
    return instance;
}

