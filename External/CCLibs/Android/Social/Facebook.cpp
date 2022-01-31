#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "Facebook.h"
#include "SocialBasicTypes.h"

USING_NS_CC;
static const std::string CB_CLASS_NAME = "com/planemo/libs/social/FacebookManager";


namespace Social {
    
    class AndroidFacebook: public Facebook {

    public:
        InvitationCallback invitationCallback = nullptr;
        ShareCompletion shareCompletionCallback = nullptr;
        LoginCallback loginCallback = nullptr;

    private:

    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for FacebookManager::getInstance");
        }
        jobject jGoogleAnalytics = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jGoogleAnalytics;
    }

    public:
        AndroidFacebook() {
            
        }
        
        virtual ~AndroidFacebook() {}
        
        virtual void start() override {
            JNIEnv *env = JniHelper::getEnv();
            JniMethodInfo startMethodInfo;
            if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "init", "()V")) {
                CCASSERT(false, "ERROR: Can't find signature for FacebookManager::init");
            }   
            env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID);
            env->DeleteLocalRef(startMethodInfo.classID);
        }
        
        virtual void loginWithReadPermissions(const std::vector<std::string> &permissions, LoginCallback callback) override {
            loginCallback = callback;
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "loginWithReadPermissions", "(Ljava/util/List;)V")) {
                CCASSERT(false, "Can't find signature for FacebookManager::loginWithReadPermissions");
                return;
            }

            jobject jlist = JniSupport::StdStringVectorToJavaStringArrayList(permissions);
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jlist);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jlist);
        }
        
        virtual void logout() override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "logout", "()V")) {
               CCASSERT(false, "Can't find signature for FacebookManager::logout");
               return;
            }   
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        
        virtual bool isLoggedIn() const override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "isLoggedIn", "()Z")) {
               CCASSERT(false, "Can't find signature for FacebookManager::isLoggedIn");
               return false;
            }   
            bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return ret;
        }
        
        virtual UserInfoPtr getUserInfo() override {
            //TODO test
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getUserInfo", "()[Ljava/lang/Object;")) {
               CCASSERT(false, "Can't find signature for FacebookManager::getUserInfo");
               return nullptr;
            }   
         
            jobjectArray jarray = (jobjectArray)methodInfo.env->CallObjectMethod(getJavaInstance(), methodInfo.methodID);
            std::vector<std::string> list = JniSupport::ArrayJavaObjectsToVectorString(jarray);
            auto userInfo = std::make_shared<UserInfo>();
            userInfo->name = list.at(0);
            userInfo->firstName = list.at(1);
            userInfo->lastName = list.at(2);
            userInfo->avatarUrl = list.at(3);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jarray);
            return userInfo;
        }
        
        virtual bool shareLink(const std::string &link, const std::string &title, 
            const std::string &imageUrl, const std::string &contentDescription, 
            ShareCompletion completion) override {
            shareCompletionCallback = completion;

            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "shareLink", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
               CCASSERT(false, "Can't find signature for FacebookManager::shareLink");
               return false;
            }   
            jstring jlink = methodInfo.env->NewStringUTF(link.c_str());
            jstring jtitle = methodInfo.env->NewStringUTF(title.c_str());
            jstring jimageUrl = methodInfo.env->NewStringUTF(imageUrl.c_str());
            jstring jcontentDescription = methodInfo.env->NewStringUTF(contentDescription.c_str());
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jlink, jtitle, jimageUrl, jcontentDescription);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jlink);
            methodInfo.env->DeleteLocalRef(jtitle);
            methodInfo.env->DeleteLocalRef(jimageUrl);
            methodInfo.env->DeleteLocalRef(jcontentDescription);

            return true;
        }
        
        virtual bool canInviteFriends() const override{
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "canInviteFriends", "()Z")) {
               CCASSERT(false, "Can't find signature for FacebookManager::canInviteFriends");
               return false;
            }   
            bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return ret;
        }

        virtual bool inviteFriends(const std::string &appLinkUrl, InvitationCallback callback) override {
            invitationCallback = callback;
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "inviteFriends", "(Ljava/lang/String;)V")) {
               CCASSERT(false, "Can't find signature for FacebookManager::inviteFriends");
               return false;
            }   
            jstring jappLinkUrl = methodInfo.env->NewStringUTF(appLinkUrl.c_str());
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jappLinkUrl);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jappLinkUrl);

            return true;
        }
    };
    
    Facebook *Facebook::getInstance() {
        static Facebook *instance = new AndroidFacebook;
        return instance;  
    }
    
}


#ifdef __cplusplus
extern "C" {
#endif

    void Java_com_planemo_libs_social_FacebookManager_00024Listener_onInviteCallback(JNIEnv *env, jobject thiz, jboolean isSuccess) {
        Social::AndroidFacebook *fb = dynamic_cast<Social::AndroidFacebook *>(Social::Facebook::getInstance());
        if (fb->invitationCallback) {
            CCLOG("FacebookManager_00024Listener_onInviteCallback ");
            fb->invitationCallback((bool)isSuccess);
            fb->invitationCallback = nullptr;
        }
    }

    void Java_com_planemo_libs_social_FacebookManager_00024Listener_onShareCompletionCallback(JNIEnv *env, jobject thiz, jboolean isSuccess) {
        Social::AndroidFacebook *fb = dynamic_cast<Social::AndroidFacebook *>(Social::Facebook::getInstance());
        if (fb->shareCompletionCallback) {
            CCLOG("FacebookManager_00024Listener_shareCompletionCallback ");
            fb->shareCompletionCallback((bool)isSuccess);
            fb->shareCompletionCallback = nullptr;
        }
    }

    void Java_com_planemo_libs_social_FacebookManager_00024Listener_onLoginCallbackFailure(JNIEnv *env, jobject thiz, jstring error) {
        Social::AndroidFacebook *fb = dynamic_cast<Social::AndroidFacebook *>(Social::Facebook::getInstance());
        if (fb->loginCallback) {
            CCLOG("FacebookManager_00024Listener_loginCallbackFailure");
            fb->loginCallback(Social::LoginResult::failure(JniHelper::jstring2string(error)));
            fb->loginCallback = nullptr;
        }
    }

    void Java_com_planemo_libs_social_FacebookManager_00024Listener_onLoginCallbackCancellation(JNIEnv *env, jobject thiz) {
        Social::AndroidFacebook *fb = dynamic_cast<Social::AndroidFacebook *>(Social::Facebook::getInstance());
        if (fb->loginCallback) {
            CCLOG("FacebookManager_00024Listener_loginCallbackCancellation");
            fb->loginCallback(Social::LoginResult::cancellation());
            fb->loginCallback = nullptr;
        }
    }


    void Java_com_planemo_libs_social_FacebookManager_00024Listener_onLoginCallbackSuccess(JNIEnv *env, jobject thiz, jobjectArray permissions) {
        Social::AndroidFacebook *fb = dynamic_cast<Social::AndroidFacebook *>(Social::Facebook::getInstance());
        if (fb->loginCallback) {
            CCLOG("FacebookManager_00024Listener_loginCallbackSuccess");
            std::vector<std::string> grantedPermissions = JniSupport::ArrayJavaObjectsToVectorString(permissions);
            fb->loginCallback(Social::LoginResult::success(grantedPermissions));
            fb->loginCallback = nullptr;
        }
    }


#ifdef __cplusplus
}
#endif


