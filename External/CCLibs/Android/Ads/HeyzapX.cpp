
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "HeyzapX.h"

USING_NS_CC;

static const std::string HEYZAP_CLASS = "com/planemo/libs/ads/HeyzapX";
static const std::string BANNER_CLASS = "com/planemo/libs/ads/HeyzapX$BannerX";

namespace Advertise {

    const std::string HeyzapX::DEFAULT_TAG = "default";
    
    class AndroidHeyzapX: public HeyzapX {
    public:
        std::function<void(bool)> rewardedVideoLoadCompletion;
        std::function<void(bool success)> rewardedVideoViewCompletion;

    private:
        bool callStringToBoolMethod(const std::string &name, const std::string &tag) const {
            jobject obj = getJavaInstance();

            JniMethodInfo methodInfo;
            if (!JniHelper::getMethodInfo(methodInfo, HEYZAP_CLASS.c_str(), name.c_str(), "(Ljava/lang/String;)Z")) {
                CCASSERT(false, ("Can't find signature for HeyzapX::" + name).c_str());
                return false;
            }

            jstring jtag = methodInfo.env->NewStringUTF(tag.c_str());
            jboolean ret = methodInfo.env->CallBooleanMethod(obj, methodInfo.methodID, jtag);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jtag);

            return (bool)ret;
        }

        void callStringToVoidMethod(const std::string &name, const std::string &tag) {
            jobject obj = getJavaInstance();

            JniMethodInfo methodInfo;
            if (!JniHelper::getMethodInfo(methodInfo, HEYZAP_CLASS.c_str(), name.c_str(), "(Ljava/lang/String;)V")) {
                CCASSERT(false, ("Can't find signature for HeyzapX::" + name).c_str());
                return;
            }

            jstring jtag = methodInfo.env->NewStringUTF(tag.c_str());
            methodInfo.env->CallVoidMethod(obj, methodInfo.methodID, jtag);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jtag);
        }

        jobject getJavaInstance() const {
            JniMethodInfo methodInfo;
            const std::string methodSignature = "()L" + HEYZAP_CLASS + ";";
            if (! JniHelper::getStaticMethodInfo(methodInfo, HEYZAP_CLASS.c_str(), "getInstance", methodSignature.c_str())) {
                CCASSERT(false, "ERROR: Can't find method for HeyzapX::getInstance");
            }
            jobject jchartboost = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return jchartboost;
        }

    public:
        AndroidHeyzapX() {

        }
        
        virtual void startSession(const std::string &publisherId) override {
            JNIEnv *env = JniHelper::getEnv();

            JniMethodInfo startMethodInfo;
            const std::string startMethodSignature = "(Ljava/lang/String;)V";
            if (! JniHelper::getMethodInfo(startMethodInfo, HEYZAP_CLASS.c_str(), "start", startMethodSignature.c_str())) {
                CCASSERT(false, "ERROR: Can't find signature for ChartboostX::start");
            }
            jstring jpublisherId = env->NewStringUTF(publisherId.c_str());

            env->CallVoidMethod(getJavaInstance(), startMethodInfo.methodID, jpublisherId);
            env->DeleteLocalRef(startMethodInfo.classID);
            env->DeleteLocalRef(jpublisherId);
        }

        virtual void loadInterstitial(const std::string &tag) override {
            callStringToVoidMethod("loadInterstitial", tag);
        }

        virtual void presentInterstitial(const std::string &tag) override {
            callStringToVoidMethod("presentInterstitial", tag);
        }

        virtual Status getInterstitialStatus(const std::string &tag) override {
            if (isInterstitialReady(tag)) {
                return Status::LOADED;
            } else if (isInterstitialLoading(tag)) {
                return Status::LOADING;
            } else {
                return Status::NONE;
            }
        }

        bool isInterstitialReady(const std::string &tag) const {
            return callStringToBoolMethod("isInterstitialReady", tag);
        }

        bool isInterstitialLoading(const std::string &tag) const {
            return callStringToBoolMethod("isInterstitialLoading", tag);
        }



        virtual void loadRewardedVideo(const std::string &tag, std::function<void(bool)> completion) override {
            rewardedVideoLoadCompletion = completion;
            callStringToVoidMethod("loadRewardedVideo", tag);
        }

        virtual void presentRewardedVideo(const std::string &tag, std::function<void(bool success)> completion) override {
            rewardedVideoViewCompletion = completion;
            callStringToVoidMethod("presentRewardedVideo", tag);            
        }

        virtual Status getRewardedVideoStatus(const std::string &tag) const override {
            if (isRewardedVideoReady(tag)) {
                return Status::LOADED;
            } else if (isRewardedVideoLoading(tag)) {
                return Status::LOADING;
            } else {
                return Status::NONE;
            }
        }

        bool isRewardedVideoReady(const std::string &tag) const {
            return callStringToBoolMethod("isRewardedVideoReady", tag);
        }

        bool isRewardedVideoLoading(const std::string &tag) const {
            return callStringToBoolMethod("isRewardedVideoLoading", tag);
        }
    };
    
    HeyzapX *HeyzapX::getInstance() {
        static HeyzapX *instance = new AndroidHeyzapX;
        return instance;
    }
    
#pragma mark -
#pragma mark Banner
    
    class AndroidHeyzapBanner: public HeyzapBanner {       
    private:
        jlong jobjectuid;

    public:
        AndroidHeyzapBanner(const std::string &pTag): HeyzapBanner(pTag) {
            JNIEnv *env = JniHelper::getEnv();

            //Create java banner
            JniMethodInfo constructorInfo;
            JniHelper::getMethodInfo(constructorInfo, BANNER_CLASS.c_str(), "<init>", "(Ljava/lang/String;)V");
            jstring jidentifier = env->NewStringUTF(pTag.c_str());
            jobject obj = env->NewObject(constructorInfo.classID, constructorInfo.methodID, jidentifier);
            env->DeleteLocalRef(constructorInfo.classID);
            env->DeleteLocalRef(jidentifier);

            jobjectuid = JniSupport::keepObject(obj);
        }
        
        virtual ~AndroidHeyzapBanner() {
            remove();
            JniSupport::releaseObject(jobjectuid);  
        }
        
        virtual void remove() override {
            jobject obj = JniSupport::getObject(jobjectuid);
            if (obj == nullptr) {
                return;
            }

            JniMethodInfo methodInfo;
            if (!JniHelper::getMethodInfo(methodInfo, BANNER_CLASS.c_str(), "remove", "()V")) {
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
            if (!JniHelper::getMethodInfo(methodInfo, BANNER_CLASS.c_str(), "present", "(FFF)V")) {
                CCASSERT(false, "Can't find signature for AdMobBanner::present");
                return;
            }

            methodInfo.env->CallVoidMethod(obj, methodInfo.methodID, (jfloat)xPercent, (jfloat)yPercent, (jfloat)yAnchor);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    };
    
    std::shared_ptr<Banner> HeyzapBanner::create(const std::string &tag){
        return std::static_pointer_cast<Banner>(std::make_shared<AndroidHeyzapBanner>(tag));
    }
}


#ifdef __cplusplus
extern "C" {
#endif

void Java_com_planemo_libs_ads_HeyzapX_00024Listener_onAdWillShow(JNIEnv *env, jobject thiz) {
    Advertise::AndroidHeyzapX *heyzap = dynamic_cast<Advertise::AndroidHeyzapX *>(Advertise::HeyzapX::getInstance());
    auto appearListener = heyzap->getAppearListener();
    if (appearListener) {
        appearListener->adWillShow();
    }
}

void Java_com_planemo_libs_ads_HeyzapX_00024Listener_onAdWillHide(JNIEnv *env, jobject thiz, jboolean rewardedVideo) {
    Advertise::AndroidHeyzapX *heyzap = dynamic_cast<Advertise::AndroidHeyzapX *>(Advertise::HeyzapX::getInstance());
    auto appearListener = heyzap->getAppearListener();
    if (appearListener) {
        appearListener->adWillHide((bool)rewardedVideo);
    }
}

void Java_com_planemo_libs_ads_HeyzapX_00024Listener_onRewardedVideoLoadCompletion(JNIEnv *env, jobject thiz, jboolean success) {
    Advertise::AndroidHeyzapX *heyzap = dynamic_cast<Advertise::AndroidHeyzapX *>(Advertise::HeyzapX::getInstance());
    if (heyzap->rewardedVideoLoadCompletion) {
        heyzap->rewardedVideoLoadCompletion((bool)success);
        heyzap->rewardedVideoLoadCompletion = nullptr;
    }
}

void Java_com_planemo_libs_ads_HeyzapX_00024Listener_onRewardedVideoViewCompletion(JNIEnv *env, jobject thiz, jboolean success) {
    Advertise::AndroidHeyzapX *heyzap = dynamic_cast<Advertise::AndroidHeyzapX *>(Advertise::HeyzapX::getInstance());
    if (heyzap->rewardedVideoViewCompletion) {
        heyzap->rewardedVideoViewCompletion((bool)success);
        heyzap->rewardedVideoViewCompletion = nullptr;
    }
}

#ifdef __cplusplus
}
#endif