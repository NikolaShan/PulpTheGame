
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "ChartboostX.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/ads/ChartboostX";
static const std::string CB_LISTENER_CLASS_NAME = CB_CLASS_NAME + "$Listener";

class AndroidChartboostX: public Advertise::ChartboostX  {
public:
    std::function<void(bool success)> completionCallbackRewardedVideo = nullptr;
    std::function<void()> completionCallbackInterstitial = nullptr;


    AndroidChartboostX() {

    }
    
    virtual ~AndroidChartboostX() {
        
    }
     
private:  
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for ChartboostX::getInstance");
        }
        jobject jchartboost = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jchartboost;
    }

    void callJavaVoidMethodWithLocation(const std::string &method, const std::string &location) const {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), method.c_str(), "(Ljava/lang/String;)V")) {
            CCASSERT(false, ("Can't find signature for ChartboostX::" + method).c_str());
            return;
        }

        jstring argJStr = methodInfo.env->NewStringUTF(location.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, argJStr);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(argJStr);
    }

    bool callJavaBoolMethodWithLocation(const std::string &method, const std::string &location) const {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), method.c_str(), "(Ljava/lang/String;)Z")) {
            CCASSERT(false, ("Can't find signature for ChartboostX::" + method).c_str());
            return false;
        }

        jstring argJStr = methodInfo.env->NewStringUTF(location.c_str());
        bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID, argJStr);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(argJStr);
        return ret;
    }

public:
    virtual void startSession(const std::string &appId, const std::string &appSignature) override {
    	JNIEnv *env = JniHelper::getEnv();

     	JniMethodInfo startMethodInfo;
    	const std::string startMethodSignature = "(Ljava/lang/String;Ljava/lang/String;" ")V";
    	if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "start", startMethodSignature.c_str())) {
    		CCASSERT(false, "ERROR: Can't find signature for ChartboostX::start");
    	}
    	jstring jappId = env->NewStringUTF(appId.c_str());
    	jstring jappSignature = env->NewStringUTF(appSignature.c_str());

    	env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, jappId, jappSignature);

    	env->DeleteLocalRef(startMethodInfo.classID);
    	env->DeleteLocalRef(jappId);
    	env->DeleteLocalRef(jappSignature);
    }

    virtual bool hasInterstitial(const Location &location) const override {
        return callJavaBoolMethodWithLocation("hasInterstitial", location);
    }

    virtual void loadInterstitial(const Location &location) override {
        callJavaVoidMethodWithLocation("cacheInterstitial", location);
    }

    virtual void presentInterstitial(const Location &location) override {
       
        callJavaVoidMethodWithLocation("presentInterstitial", location);
    }

    virtual bool hasRewardedVideo(const Location &location) const override {
        return callJavaBoolMethodWithLocation("hasRewardedVideo", location);
    }

    virtual void loadRewardedVideo(const Location &location) override {
        callJavaVoidMethodWithLocation("cacheRewardedVideo", location);
    }

    virtual void presentRewardedVideo(const Location &location, std::function<void(bool success)> completion) override {
        completionCallbackRewardedVideo = completion;
        callJavaVoidMethodWithLocation("presentRewardedVideo", location);
    }

/*    virtual bool hasMoreApps(const Location &location) const override {
        return ;
    }
    
    virtual void loadMoreApps(const Location &location) override {
        
    }
    
    virtual void presentMoreApps(const Location &location) override {
        
    }*/
};

Advertise::ChartboostX *Advertise::ChartboostX::getInstance() {
    static ChartboostX *instance = new AndroidChartboostX();
    return instance;
}


#pragma mark - JNI
   
static Advertise::ChartboostX::Delegate *getCppChartboostDelegate() {
    return Advertise::ChartboostX::getInstance()->getDelegate();
}

static void completionCallbackInterstitial() {
    
}


#ifdef __cplusplus
extern "C" {
#endif

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didCacheInterstitial(JNIEnv *env, jobject thiz, jstring jlocation) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didLoadInterstitial(JniHelper::jstring2string(jlocation));
    }
}

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didFailToLoadInterstitial(JNIEnv *env, jobject thiz, jstring jlocation, jint error) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didFailToLoadInterstitial(JniHelper::jstring2string(jlocation), error);
    }
}

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didCacheMoreApps(JNIEnv *env, jobject thiz, jstring jlocation) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didLoadMoreApps(JniHelper::jstring2string(jlocation));
    }
}

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didFailToLoadMoreApps(JNIEnv *env, jobject thiz, jstring jlocation, jint error) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didFailToLoadMoreApps(JniHelper::jstring2string(jlocation), error);
    }
}

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didCacheRewardedVideo(JNIEnv *env, jobject thiz, jstring jlocation) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didLoadRewardedVideo(JniHelper::jstring2string(jlocation));
    }
}

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didFailToLoadRewardedVideo(JNIEnv *env, jobject thiz, jstring jlocation, jint error) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didFailToLoadRewardedVideo(JniHelper::jstring2string(jlocation), error);
    }
}
 
void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didCompleteRewardedVideo(JNIEnv *env, jobject thiz, jstring jlocation, jint reward) {
    if (auto cppDelegate = getCppChartboostDelegate()) {
        cppDelegate->didCompleteRewardedVideo(JniHelper::jstring2string(jlocation));
    }
   
    auto cb = dynamic_cast<AndroidChartboostX *> (Advertise::ChartboostX::getInstance());
    if(cb->completionCallbackRewardedVideo){
       cb->completionCallbackRewardedVideo(true);
       cb->completionCallbackRewardedVideo = nullptr;
    }
}   

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didCloseInterstitial(JNIEnv *env, jobject thiz) {
     completionCallbackInterstitial();
}

void Java_com_planemo_libs_ads_ChartboostX_00024Listener_didCloseRewardedVideo(JNIEnv *env, jobject thiz) {
    auto cb = dynamic_cast<AndroidChartboostX *> (Advertise::ChartboostX::getInstance());
    if(cb->completionCallbackRewardedVideo){
       cb->completionCallbackRewardedVideo(true);
       cb->completionCallbackRewardedVideo = nullptr;
    }
}

#ifdef __cplusplus
}
#endif















