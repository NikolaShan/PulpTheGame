#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "GoogleAnalytics.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/analytics/GoogleAnalyticsManager";

class AndroidGoogleAnalytics: public GoogleAnalytics {
private:
    void callJavaVoidMethodWithBoolean(const std::string &method, bool flag) const {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), method.c_str(), "(Z)V")) {
            CCASSERT(false, ("Can't find signature for GoogleAnalytics::" + method).c_str());
            return;
        }
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, flag);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    
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
    virtual void startWithTrackerId(const std::string &tracker) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "startWithTrackerId", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for GoogleAnalyticsManager::startWithTrackerId");
        }
        jstring jtracker = env->NewStringUTF(tracker.c_str());
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jtracker);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jtracker);
    }
    
    virtual void enableDebugLogging(bool enable) override {
        callJavaVoidMethodWithBoolean("setEnableDebugLogging", enable);
    }
    
    virtual void enableIDFACollection(bool enable) override {
        callJavaVoidMethodWithBoolean("enableAdvertisingIdCollection", enable);
    }
    
    virtual void setDispatchInterval(float seconds) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "setDispatchInterval", "(F)V")) {
            CCASSERT(false, "Can't find signature for GoogleAnalytics::setDispatchInterval");
            return;
        }
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, seconds);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    
    virtual void set(const std::string &key, const std::string &value) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "set", "(Ljava/lang/String;Ljava/lang/String;)V")) {
            CCASSERT(false, ("Can't find signature for GoogleAnalyticsManager::set"));
            return;
        }

        jstring jkey = methodInfo.env->NewStringUTF(key.c_str());
        jstring jvalue = methodInfo.env->NewStringUTF(value.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jkey, jvalue);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jkey);
        methodInfo.env->DeleteLocalRef(jvalue);
    }
    
    //sample: (ui_action, button_press, play, Value::Null)
    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "sendEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
            CCASSERT(false, "Can't find signature for GoogleAnalytics::sendEvent" );
            return;
        }
        jstring jcategory= env->NewStringUTF(category.c_str());
        jstring jaction = env->NewStringUTF(action.c_str());
        jstring jlabel = env->NewStringUTF(label.c_str());    
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jcategory, jaction, jlabel);          
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        env->DeleteLocalRef(jcategory);
        env->DeleteLocalRef(jaction);
        env->DeleteLocalRef(jlabel);
    }
    
    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label,
                           int value) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo; 
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "sendEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V")) {
            CCASSERT(false, "Can't find signature for GoogleAnalytics::sendEvent with value" );
            return;
        }
   
        jstring jcategory= env->NewStringUTF(category.c_str());
        jstring jaction = env->NewStringUTF(action.c_str());
        jstring jlabel = env->NewStringUTF(label.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jcategory, jaction, jlabel, value);       
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        env->DeleteLocalRef(jcategory);
        env->DeleteLocalRef(jaction);
        env->DeleteLocalRef(jlabel);
    }
    
    //sample: (main_menu)
    virtual void setScreen(const std::string &screen) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "setScreen", "(Ljava/lang/String;)V")) {
            CCASSERT(false, ("Can't find signature for GoogleAnalyticsManager::setScreen"));
            return;
        }
        jstring jscreen = methodInfo.env->NewStringUTF(screen.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jscreen);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jscreen);
    }
    
    //sample: (resources, loadTime, high_scores, "")
    virtual void sendTiming(const std::string &category,
                            long intervalMs,
                            const std::string &name,
                            const std::string &label = "") override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "sendTiming", "(Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;)V")) {
             CCASSERT(false, ("Can't find signature for GoogleAnalyticsManager::sendTiming"));
             return;
        }
     
        jstring jcategory = methodInfo.env->NewStringUTF(category.c_str());
        jstring jname = methodInfo.env->NewStringUTF(name.c_str());
        jstring jlabel = methodInfo.env->NewStringUTF(label.c_str());
        jlong jintervalMs =  intervalMs; //because с++ long 4 byte, java long 8 byte
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jcategory, jintervalMs, jname, jlabel);       
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jcategory);
        methodInfo.env->DeleteLocalRef(jname);
        methodInfo.env->DeleteLocalRef(jlabel);
    }
};


std::shared_ptr<GoogleAnalytics> GoogleAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<GoogleAnalytics>(std::make_shared<AndroidGoogleAnalytics>());
    return instance;
}