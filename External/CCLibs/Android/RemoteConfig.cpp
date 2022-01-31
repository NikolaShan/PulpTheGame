#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "JniSupport.h"
#include "Utils/RemoteConfig.h"
#include "Utils/JsonValueConverter.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/RemoteConfig";

class RemoteConfig::Implementation {
private:

public:
    Implementation() {

    }

    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for RemoteConfig::getInstance");
        }
        jobject jinstance = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jinstance;
    }

    void fetch(float expirationDurationInSeconds) {
    	JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "fetch", "(F)V")) {
            CCASSERT(false, "ERROR: Can't find method for RemoteConfig::fetch");
            return;
        }
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, (jfloat)expirationDurationInSeconds);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    void setDefaults(const cocos2d::ValueMap &defaults) {
    	const std::string serialized = JsonValueConverter::toJsonString(cocos2d::Value(defaults));
    	JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        const std::string methodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "setDefaults", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for RemoteConfig::setDefaults");
        }
        jstring jserialized = env->NewStringUTF(serialized.c_str());
        env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jserialized);
        env->DeleteLocalRef(methodInfo.classID);
        env->DeleteLocalRef(jserialized);
    }

    std::string getStringValue(const std::string &key) const {
		JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        const std::string methodSignature = "(Ljava/lang/String;)Ljava/lang/String;";
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getStringValue", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for RemoteConfig::getStringValue");
        }
        jstring jkey = env->NewStringUTF(key.c_str());
        jstring jret = (jstring) env->CallObjectMethod(getJavaInstance(), methodInfo.methodID, jkey);
        const std::string ret = JniHelper::jstring2string(jret);
        methodInfo.env->DeleteLocalRef(jret);
        env->DeleteLocalRef(methodInfo.classID);
        env->DeleteLocalRef(jkey);
        return ret;
    }

    float getFloatValue(const std::string &key) const {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getFloatValue", "(Ljava/lang/String;)F")) {
            CCASSERT(false, "ERROR: Can't find method for RemoteConfig::getFloatValue");
            return false;
        }
        jstring jkey = methodInfo.env->NewStringUTF(key.c_str());
        jfloat ret = methodInfo.env->CallFloatMethod(getJavaInstance(), methodInfo.methodID, jkey);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jkey);
        return (float)ret;
    }

    int getIntValue(const std::string &key) const {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getIntValue", "(Ljava/lang/String;)I")) {
            CCASSERT(false, "ERROR: Can't find method for RemoteConfig::getIntValue");
            return false;
        }
        jstring jkey = methodInfo.env->NewStringUTF(key.c_str());
        jint ret = methodInfo.env->CallIntMethod(getJavaInstance(), methodInfo.methodID, jkey);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jkey);
        return (int)ret;
    }
    
    bool getBoolValue(const std::string &key) const {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getBoolValue", "(Ljava/lang/String;)Z")) {
            CCASSERT(false, "ERROR: Can't find method for RemoteConfig::getBoolValue");
            return false;
        }
        jstring jkey = methodInfo.env->NewStringUTF(key.c_str());
        jboolean ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID, jkey);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jkey);
        return (bool)ret;
    }
};

RemoteConfig::RemoteConfig() {
    pimpl = ImplPtr(new Implementation());
}

void RemoteConfig::fetch(float expirationDurationInSeconds) {
    pimpl->fetch(expirationDurationInSeconds);
}

void RemoteConfig::setDefaults(const cocos2d::ValueMap &defaults) {
    pimpl->setDefaults(defaults);
}

std::string RemoteConfig::getStringValue(const std::string &key) const {
    return pimpl->getStringValue(key);
}

float RemoteConfig::getFloatValue(const std::string &key) const {
    return pimpl->getFloatValue(key);
}

int RemoteConfig::getIntValue(const std::string &key) const {
    return pimpl->getIntValue(key);
}

bool RemoteConfig::getBoolValue(const std::string &key) const {
    return pimpl->getBoolValue(key);
}