#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "Appodeal.h"

USING_NS_CC;
using namespace Advertise;

static const std::string APPODEAL_CLASS = "com/planemo/libs/ads/AppodealManager";

class AndroidAppodeal: public AppodealAds {

public:
     std::function<void(bool success)> completionRewardedCallback = nullptr;    


private:
	jobject getJavaInstance() const {
		JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + APPODEAL_CLASS + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, APPODEAL_CLASS.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for Appodeal::getInstance");
        }
        jobject instance = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return instance;
	}

	void callVoidNoArgsMethod(const std::string &name) {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, APPODEAL_CLASS.c_str(), name.c_str(), "()V")) {
            CCASSERT(false, ("ERROR: Can't find signature for Appodeal::" + name).c_str());
        }
        env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
        env->DeleteLocalRef(methodInfo.classID);
	}

	bool callBooleanNoArgsMethod(const std::string &name) const {
		JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, APPODEAL_CLASS.c_str(), name.c_str(), "()Z")) {
            CCASSERT(false, ("ERROR: Can't find signature for Appodeal::" + name).c_str());
        }
        jboolean ret = env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID);
        env->DeleteLocalRef(methodInfo.classID);
        return ret;
	}

public:
    virtual void startWithAppKey(const std::string &key) override {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;
        const std::string startMethodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getMethodInfo(startMethodInfo, APPODEAL_CLASS.c_str(), "initialize", startMethodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for Appodeal::initialize");
        }
        jstring jappKey = env->NewStringUTF(key.c_str());
        env->CallVoidMethod(getJavaInstance(), startMethodInfo.methodID, jappKey);
        env->DeleteLocalRef(startMethodInfo.classID);
        env->DeleteLocalRef(jappKey);
    }
       
    virtual void removeBanner() override {
        callVoidNoArgsMethod("removeBanner");
    }   

    virtual void presentBanner(float xPercent, float yPercent, float yAnchor) {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, APPODEAL_CLASS.c_str(), "presentBannerAtPosition", "(FFF)V")) {
            CCASSERT(false, "ERROR: Can't find signature for Appodeal::presentBannerAtPosition");
        }
        env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, (jfloat)xPercent, (jfloat)yPercent, (jfloat)yAnchor);
        env->DeleteLocalRef(methodInfo.classID);
    } 

    virtual void presentInterstitial() override {
    	callVoidNoArgsMethod("presentInterstitial");
    }
    
    virtual void presentInterstitialStaticOrVideo() override {
        callVoidNoArgsMethod("presentInterstitialStaticOrVideo");
    }

    virtual void presentVideoInterstitial() override {
        callVoidNoArgsMethod("presentVideoInterstitial");
    }

    virtual void presentRewardedVideo(std::function<void(bool success)> completion) override {
        completionRewardedCallback = completion;
        callVoidNoArgsMethod("presentRewardedVideo");
    }

    virtual bool hasCachedRewardedVideo() const override {
        return true;
    }
 
};

std::shared_ptr<AppodealAds> AppodealAds::getInstance() {
	static auto instance = std::dynamic_pointer_cast<AppodealAds>(std::make_shared<AndroidAppodeal>());
	return instance;
}

#ifdef __cplusplus
extern "C" {
#endif

    void Java_com_planemo_libs_ads_AppodealManager_00024Listener_onAdWillShow(JNIEnv *env, jobject thiz) {
        auto ad = Advertise::AppodealAds::getInstance();
        auto appearListener = ad->getAppearListener();
        if (appearListener) {
            CCLOG("AppodealManager_00024Listener_onAdWhillShow");
            appearListener->adWillShow();
        }
    }

    void Java_com_planemo_libs_ads_AppodealManager_00024Listener_onAdWillHide(JNIEnv *env, jobject thiz, jboolean rewardedVideo) {
        auto ad = Advertise::AppodealAds::getInstance();
        auto appearListener = ad->getAppearListener();
        if (appearListener) {
            CCLOG("AppodealManager_00024Listener_onAdWhillHide");
            appearListener->adWillHide((bool)rewardedVideo);
        }
    }

    void Java_com_planemo_libs_ads_AppodealManager_00024Listener_onRewardedVideoCompletion(JNIEnv *env, jobject thiz, jboolean success) {
        auto ad = std::dynamic_pointer_cast<AndroidAppodeal>(Advertise::AppodealAds::getInstance());
        if (ad->completionRewardedCallback) {
            CCLOG("AppodealManager_00024Listener_onRewardedVideoCompletion");
            ad->completionRewardedCallback((bool)success);
            ad->completionRewardedCallback = nullptr;
        }
    }


#ifdef __cplusplus
}
#endif

