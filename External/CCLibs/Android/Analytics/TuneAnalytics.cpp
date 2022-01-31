#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "TuneAnalytics.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/analytics/TuneAnalytics";

class AndroidTuneAnalytics: public TuneAnalytics {
private:   
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for GoogleAnalytics::getInstance");
        }
        jobject jGoogleAnalytics = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jGoogleAnalytics;
    }

public:
    virtual void start(const std::string &advertiserId, const std::string &conversionKey, bool existingUser) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;Ljava/lang/String;)V";
        if (! JniHelper::getMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "start", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for TuneAnalytics::start");
        }
        jstring jadvertiserId = env->NewStringUTF(advertiserId.c_str());
        jstring jconversionKey = env->NewStringUTF(conversionKey.c_str());
        env->CallVoidMethod(getJavaInstance(), startMethodInfo.methodID, jadvertiserId, jconversionKey);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jadvertiserId);
        env->DeleteLocalRef(jconversionKey);

        if (existingUser) {
            setExistingUser();
        }
    }
    
    void setExistingUser() {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "setExistingUser", "()V")) {
            CCASSERT(false, "Can't find signature for TuneAnalytics::setExistingUser");
            return;
        }
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    virtual void measureLevelAchieved(int levelNumber) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo; 
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "measureLevelAchieved", "(I)V")) {
            CCASSERT(false, "Can't find signature for TuneAnalytics::measureLevelAchieved");
            return;
        }
   
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, (jint)levelNumber);       
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }    
};


std::shared_ptr<TuneAnalytics> TuneAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<TuneAnalytics>(std::make_shared<AndroidTuneAnalytics>());
    return instance;
}