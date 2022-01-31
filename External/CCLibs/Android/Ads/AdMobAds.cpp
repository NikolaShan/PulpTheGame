
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "AdMobAds.h"

USING_NS_CC;

static const std::string BANNER_CLASS = "com/planemo/libs/ads/AdMobBanner";
static const std::string INTERSTITIAL_CLASS = "com/planemo/libs/ads/AdMobInterstitial";
static const std::string REWARDED_VIDEO_CLASS = "com/planemo/libs/ads/AdMobRewardedVideo";
static const std::string ADMOB_MANAGER_CLASS = "com/planemo/libs/ads/AdMobManager";

void Advertise::AdMobManager::start(const std::string &appId) {
    JniMethodInfo methodInfo;
    const std::string methodSignature = "(Ljava/lang/String;)V";
    if (! JniHelper::getStaticMethodInfo(methodInfo, ADMOB_MANAGER_CLASS.c_str(), "start", methodSignature.c_str())) {
        CCASSERT(false, "ERROR: Can't find method for AdMobManager::start");
    }
    jstring jAppId = methodInfo.env->NewStringUTF(appId.c_str());
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAppId);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(jAppId);
}

class AndroidAdMobInterstitial: public Advertise::AdMobInterstitial {

private:
    jlong jobjectuid;
    
private:
    bool interstitialCallEmptyBoolMethod(const std::string &name) const {
        jobject obj = JniSupport::getObject(jobjectuid);
        if (obj == nullptr) {
            return false;
        }

        JniMethodInfo methodInfo;
        if (!JniHelper::getMethodInfo(methodInfo, INTERSTITIAL_CLASS.c_str(), name.c_str(), "()Z")) {
            CCASSERT(false, ("Can't find signature for AdMobInterstitial::" + name).c_str());
            return false;
        }

        jboolean ret = methodInfo.env->CallBooleanMethod(obj, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return (bool)ret;
    }

    void interstitialCallEmptyVoidMethod(const std::string &name) {
        jobject obj = JniSupport::getObject(jobjectuid);
        if (obj == nullptr) {
            return;
        }

        JniMethodInfo methodInfo;
        if (!JniHelper::getMethodInfo(methodInfo, INTERSTITIAL_CLASS.c_str(), name.c_str(), "()V")) {
            CCASSERT(false, ("Can't find signature for AdMobInterstitial::" + name).c_str());
            return;
        }

        methodInfo.env->CallVoidMethod(obj, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    
public:
    AndroidAdMobInterstitial(const std::string &identifier): AdMobInterstitial(identifier) {
        JNIEnv *env = JniHelper::getEnv();

        long address =  reinterpret_cast<long>(this);
        jlong pointerToNativeListener = address;

        //Create java banner
        JniMethodInfo constructorInfo;
        JniHelper::getMethodInfo(constructorInfo, INTERSTITIAL_CLASS.c_str(), "<init>", "(Ljava/lang/String;J)V");
        jstring jidentifier = env->NewStringUTF(identifier.c_str());
        jobject obj = env->NewObject(constructorInfo.classID, constructorInfo.methodID, jidentifier, pointerToNativeListener);
        env->DeleteLocalRef(constructorInfo.classID);
        env->DeleteLocalRef(jidentifier);

        jobjectuid = JniSupport::keepObject(obj);
    }
    
    virtual ~AndroidAdMobInterstitial() {
        interstitialCallEmptyVoidMethod("prepareForRelease");
        JniSupport::releaseObject(jobjectuid); 
    }
    
    virtual void load() override {
        interstitialCallEmptyVoidMethod("load");
    }
    
    virtual void present() override {
        interstitialCallEmptyVoidMethod("present");
    }
    
    virtual Advertise::Status getCurrentStatus() const override {
        if (isLoaded()) {
            return Advertise::Status::LOADED;
        } else if (isLoading()) {
            return Advertise::Status::LOADING;
        } else {
            return Advertise::Status::NONE;
        }
    }

    bool isLoaded() const {
        return interstitialCallEmptyBoolMethod("isReady");
    }

    bool isLoading() const {
        return interstitialCallEmptyBoolMethod("isLoading");
    }

};

std::shared_ptr<Advertise::Interstitial> Advertise::AdMobInterstitial::create(const std::string &identifier) {
    return std::dynamic_pointer_cast<Advertise::Interstitial>(std::make_shared<AndroidAdMobInterstitial>(identifier));
}



class AndroidAdMobBanner: public Advertise::AdMobBanner {
private:
    jlong jobjectuid;
   
public:
    
    AndroidAdMobBanner(const std::string &identifier): AdMobBanner(identifier) {
        JNIEnv *env = JniHelper::getEnv();

        //Create java banner
        JniMethodInfo constructorInfo;
        JniHelper::getMethodInfo(constructorInfo, BANNER_CLASS.c_str(), "<init>", "(Ljava/lang/String;)V");
        jstring jidentifier = env->NewStringUTF(identifier.c_str());
        jobject obj = env->NewObject(constructorInfo.classID, constructorInfo.methodID, jidentifier);
        env->DeleteLocalRef(constructorInfo.classID);
        env->DeleteLocalRef(jidentifier);

        jobjectuid = JniSupport::keepObject(obj);
    }
    
    virtual ~AndroidAdMobBanner() {
        remove();
        JniSupport::releaseObject(jobjectuid);  
    }
    
    virtual void remove() override {
        jobject obj = JniSupport::getObject(jobjectuid);
        if (obj == nullptr) {
            return;
        }

        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, BANNER_CLASS.c_str(), "remove", "()V")) {
            CCASSERT(false, "Can't find signature for AdMobBanner::remove");
            return;
        }

        methodInfo.env->CallVoidMethod(obj, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    
    virtual void present(float xPercent, float yPercent, float yAnchor) override {
        jobject obj = JniSupport::getObject(jobjectuid);
        if (obj == nullptr) {
            return;
        }

        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, BANNER_CLASS.c_str(), "present", "(FFF)V")) {
            CCASSERT(false, "Can't find signature for AdMobBanner::present");
            return;
        }

        methodInfo.env->CallVoidMethod(obj, methodInfo.methodID, (jfloat)xPercent, (jfloat)yPercent, (jfloat)yAnchor);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
};

std::shared_ptr<Advertise::Banner> Advertise::AdMobBanner::create(const std::string &identifier) {
    return std::dynamic_pointer_cast<Advertise::Banner>(std::make_shared<AndroidAdMobBanner>(identifier));
}


class AndroidAdMobRewardedVideo: public Advertise::AdMobRewardedVideo {
private:
    Advertise::Status currentStatus = Advertise::Status::NONE;
    std::string adUnitId;
    std::function<void(bool success)> completion;
    std::function<void(bool success)> loadCompletion;

    bool presentWhenReady = false;

public:
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + REWARDED_VIDEO_CLASS + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, REWARDED_VIDEO_CLASS.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for AdMobRewardedVideo::getInstance");
        }
        jobject jAdMobRewardedVideo = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jAdMobRewardedVideo;
    }

    virtual void setAdUnitId(const std::string &adUnitId) override {
        this->adUnitId = adUnitId;
    }

    virtual void load() override {
        currentStatus = Advertise::Status::LOADING;
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        const std::string methodSignature = "(Ljava/lang/String;)V";
        if (! JniHelper::getMethodInfo(methodInfo, REWARDED_VIDEO_CLASS.c_str(), "load", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for AdMobRewardedVideo::load");
        }
        jstring jAdUnitId = env->NewStringUTF(adUnitId.c_str());
        env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jAdUnitId);
        env->DeleteLocalRef(methodInfo.classID);
        env->DeleteLocalRef(jAdUnitId);
    }

    virtual void load(std::function<void(bool success)> completion) override {
        loadCompletion = completion;
        load();
    }

    virtual void present(std::function<void(bool success)> completion) override {
        this->completion = completion;
        if (getCurrentStatus() == Advertise::Status::LOADED) {
            present();
        } else {
            presentWhenReady = true;
        }
    }

    void present() {
        presentWhenReady = false;
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()V";
        if (! JniHelper::getMethodInfo(methodInfo, REWARDED_VIDEO_CLASS.c_str(), "present", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find signature for AdMobRewardedVideo::present");
        }
        env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
        env->DeleteLocalRef(methodInfo.classID);
    }

    virtual Advertise::Status getCurrentStatus() const override {
        return currentStatus;
    }

    void onAdLoaded() {
        currentStatus = Advertise::Status::LOADED;
        if (loadCompletion) {
            loadCompletion(true);
            loadCompletion = nullptr;
        }
        if (presentWhenReady) {
            present();
        }
    }

    void onAdFailedToLoad() {
        currentStatus = Advertise::Status::NONE;
        if (loadCompletion) {
            loadCompletion(false);
            loadCompletion = nullptr;
        }

        presentWhenReady = false;
        if (completion) {
            completion(false);
            completion = nullptr;
        }
    }

    void onAdOpened() {
        currentStatus = Advertise::Status::NONE;
        AdsManager::getInstance()->getAppearListener()->adWillShow();
    }

    void onAdClosed() {
        AdsManager::getInstance()->getAppearListener()->adWillHide(true);
        if (completion) {
            completion(false);
            completion = nullptr;
        }
        load();
    }

    void onRewarded() {
        if (completion) {
            completion(true);
            completion = nullptr;
        }
    }
};

Advertise::AdMobRewardedVideo *Advertise::AdMobRewardedVideo::getInstance() {
    static auto ret = new AndroidAdMobRewardedVideo();
    return ret;
}


#ifdef __cplusplus
extern "C" {
#endif

    void Java_com_planemo_libs_ads_AdMobInterstitial_00024Listener_onAdWillShow(JNIEnv *env, jobject thiz, jlong pointerToInterstitialBanner) {
         CCLOG("AdMobInterstitial_00024Listener_onAdWillShow");
         AdsManager::getInstance()->getAppearListener()->adWillShow();
    }

    void Java_com_planemo_libs_ads_AdMobInterstitial_00024Listener_onAdWillHide(JNIEnv *env, jobject thiz, jlong pointerToInterstitialBanner) {
         CCLOG("AdMobInterstitial_00024Listener_onAdWillHide");
         AdsManager::getInstance()->getAppearListener()->adWillHide(false);
    }

    void Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdOpened(JNIEnv *env, jobject thiz) {
         CCLOG("Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdOpened");
         AndroidAdMobRewardedVideo *instance = dynamic_cast<AndroidAdMobRewardedVideo *>(Advertise::AdMobRewardedVideo::getInstance());
         instance->onAdOpened();
    }

    void Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdClosed(JNIEnv *env, jobject thiz) {
         CCLOG("Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdClosed");
         AndroidAdMobRewardedVideo *instance = dynamic_cast<AndroidAdMobRewardedVideo *>(Advertise::AdMobRewardedVideo::getInstance());
         instance->onAdClosed();
    }

    void Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdLoaded(JNIEnv *env, jobject thiz) {
         CCLOG("Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdLoaded");

         AndroidAdMobRewardedVideo *instance = dynamic_cast<AndroidAdMobRewardedVideo *>(Advertise::AdMobRewardedVideo::getInstance());
         instance->onAdLoaded();
    }

    void Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdFailedToLoad(JNIEnv *env, jobject thiz) {
         CCLOG("Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onAdFailedToLoad");
         AndroidAdMobRewardedVideo *instance = dynamic_cast<AndroidAdMobRewardedVideo *>(Advertise::AdMobRewardedVideo::getInstance());
         instance->onAdFailedToLoad();
    }

    void Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onRewarded(JNIEnv *env, jobject thiz) {
         CCLOG("Java_com_planemo_libs_ads_AdMobRewardedVideo_00024Listener_onRewarded");
         AndroidAdMobRewardedVideo *instance = dynamic_cast<AndroidAdMobRewardedVideo *>(Advertise::AdMobRewardedVideo::getInstance());
         instance->onRewarded();
    }


#ifdef __cplusplus
}
#endif
