#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "../JniSupport.h"
#include "FirebaseAnalytics.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/analytics/FirebaseAnalyticsManager";


class AndroidFirebaseAnalytics: public FirebaseAnalytics {
public:
    
    
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for FirebaseAnalytics::getInstance");
        }
        jobject jGoogleAnalytics = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jGoogleAnalytics;
    }

    virtual void start() override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "()V";
        if (! JniHelper::getMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "start", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FirebaseAnalytics::start");
        }
        env->CallVoidMethod(getJavaInstance(), startMethodInfo.methodID);
        env->DeleteLocalRef(startMethodInfo.classID);
    }

    virtual void sendEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo sendEventMethodInfo;
        const std::string sendEventMethodSignature = "(Ljava/lang/String;Ljava/util/Map;)V";
        if (! JniHelper::getMethodInfo(sendEventMethodInfo, CB_CLASS_NAME.c_str(), "sendEvent", sendEventMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for FirebaseAnalytics::sendEvent");
        }
        jstring jevent = env->NewStringUTF(event.c_str());
        jobject jmap = JniSupport::StdStringMapToJavaMap(parameters);
        env->CallVoidMethod(getJavaInstance(), sendEventMethodInfo.methodID, jevent, jmap);
        env->DeleteLocalRef(sendEventMethodInfo.classID);
        env->DeleteLocalRef(jevent);
        env->DeleteLocalRef(jmap);
    }
    
};

std::shared_ptr<FirebaseAnalytics> FirebaseAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<FirebaseAnalytics>(std::make_shared<AndroidFirebaseAnalytics>());
    return instance;
}