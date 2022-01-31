#include <jni.h>
#include <memory>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "AdColonyX.h"


using namespace Advertise;
USING_NS_CC;
static const std::string CB_CLASS_NAME = "com/planemo/libs/ads/AdColonyManager";

class AndroidAdColonyX: public AdColonyX {
public: 
     std::function<void(bool)> completionCallback = nullptr;


private:

    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for AdColonyManager::getInstance");
        }
        jobject jAdColony = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jAdColony;
    }
    
public:
    AndroidAdColonyX() {
        
    }
    
    virtual void start(const std::string &appId, const std::vector<std::string> &zoneIds, bool logEnabled) override {
        //logEnabled  - is missing for android implementation
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "init", "(Ljava/lang/String;Ljava/util/List;)V")) {
            CCASSERT(false, "ERROR: Can't find signature for AdColonyManager::init");
        }   
        jstring jappId = methodInfo.env->NewStringUTF(appId.c_str());
        jobject jzones = JniSupport::StdStringVectorToJavaStringArrayList(zoneIds);
        env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jappId, jzones);
        env->DeleteLocalRef(methodInfo.classID);
        env->DeleteLocalRef(jappId);
        env->DeleteLocalRef(jzones);
    }
    
    virtual bool isVirtualCurrencyRewardAvailableForZone(const std::string &zoneID) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "isVirtualCurrencyRewardAvailableForZone", "(Ljava/lang/String;)Z")) {
           CCASSERT(false, "Can't find signature for AdColonyManager::isVirtualCurrencyRewardAvailableForZone");
           return false;
        }   
        jstring jzoneID = methodInfo.env->NewStringUTF(zoneID.c_str());
        bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID, jzoneID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }
    
    virtual int getVirtualCurrencyRewardsAvailableTodayForZone(const std::string &zoneID) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getVirtualCurrencyRewardsAvailableTodayForZone", "()I")) {
           CCASSERT(false, "Can't find signature for AdColonyManager::getVirtualCurrencyRewardsAvailableTodayForZone");
           return false;
        }   
        int ret = (int) methodInfo.env->CallIntMethod(getJavaInstance(), methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }
    
    virtual int getVirtualCurrencyRewardAmountForZone(const std::string &zoneID) override {
        return 0;
    }
    
    virtual std::string getVirtualCurrencyNameForZone(const std::string &zoneID) override {
        return "";
    }
    
    virtual void playRewardedVideoForZone(const std::string &zoneID, std::function<void(bool)> completion) override {
        completionCallback = completion;
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "playRewardedVideoForZone", "(Ljava/lang/String;)V")) {
           CCASSERT(false, "Can't find signature for AdColonyManager::playRewardedVideoForZone");
           return;
        }   
        jstring jzoneID = methodInfo.env->NewStringUTF(zoneID.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jzoneID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jzoneID);
    }
    

    virtual void setDefaultInterstitialZoneID(const std::string &zoneId)  override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "setDefaultInterstitialZoneID", "(Ljava/lang/String;)V")) {
           CCASSERT(false, "Can't find signature for AdColonyManager::setDefaultInterstitialZoneID");
           return;
        }   
        jstring jzoneId = methodInfo.env->NewStringUTF(zoneId.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jzoneId);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jzoneId);
    }

    virtual void setUserMetadata(const std::string &metadataType, const std::string &value) override {
        
    }
    
};

std::shared_ptr<AdColonyX> AdColonyX::getInstance() {
    static auto instance = std::dynamic_pointer_cast<AdColonyX>(std::make_shared<AndroidAdColonyX>());
    return instance;
}

#ifdef __cplusplus
extern "C" {
#endif

    void Java_com_planemo_libs_ads_AdColonyManager_00024Listener_onCompletionCallback(JNIEnv *env, jobject thiz, jboolean isSuccess) {
        auto ad = std::dynamic_pointer_cast<AndroidAdColonyX>(Advertise::AdColonyX::getInstance());
        if (ad->completionCallback) {
            CCLOG("AdColonyManager_00024Listener_onCompletionCallback");
            ad->completionCallback((bool)isSuccess);
            ad->completionCallback = nullptr;
        }
    }

    // void Java_com_planemo_libs_ads_AdColonyManager_00024Listener_onHandleRewardCallback(JNIEnv *env, jobject thiz,  jstring currencyName, jint amount) {
    //     auto ad = std::dynamic_pointer_cast<AndroidAdColonyX>(Advertise::AdColonyX::getInstance());
    //     CCLOG("AdColonyManager_00024Listener_onHandleRewardCallback");
    //     ad->handleReward(JniHelper::jstring2string(currencyName), (int)amount);
    // }


#ifdef __cplusplus
}
#endif
