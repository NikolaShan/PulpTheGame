#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "Vkontakte.h"

USING_NS_CC;
static const std::string CB_CLASS_NAME = "com/planemo/libs/social/VkManager";

namespace Social {
    
    class AndroidVkontakte: public Vkontakte {
    public:
        LoginCallback loginCallback = nullptr;
        ShareCompletion shareCompletionCallback = nullptr;
        UserInfoCallback userInfoCallback = nullptr;
    
    private:
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for VkkManager::getInstance");
        }
        jobject jVkManager = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jVkManager;
    }
    public:
        AndroidVkontakte() {
        }

        virtual ~AndroidVkontakte() {}
        
        virtual void startWithAppId(const std::string &appId) override {
            //Appid is in AndroidManifest.xml
            JNIEnv *env = JniHelper::getEnv();
            JniMethodInfo startMethodInfo;
            if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "init", "()V")) {
                CCASSERT(false, "ERROR: Can't find signature for VkManager::init");
            }   
            env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID);
            env->DeleteLocalRef(startMethodInfo.classID);
        }
        
        virtual void loginWithPermissions(const std::vector<std::string> &permissions, LoginCallback callback) override {
            loginCallback = callback;
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "loginWithPermissions", "(Ljava/util/List;)V")) {
                CCASSERT(false, "Can't find signature for VkManager::loginWithPermissions");
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
                CCASSERT(false, "Can't find signature for VkManager::logout");
                return;
            }
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        
        virtual bool isLoggedIn() const override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "isLoggedIn", "()Z")) {
               CCASSERT(false, "Can't find signature for VkManager::isLoggedIn");
               return false;
            }   
            bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return ret;
        }

        virtual bool isVkCountry() const override{
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "isVkCountry", "()Z")) {
               CCASSERT(false, "Can't find signature for VkManager::isVkCountry");
               return false;
            }   
            bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return ret;
        }
        

        virtual void requestUserInfo(UserInfoCallback callback) override {
            userInfoCallback = callback;
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "requestUserInfo", "()V")) {
               CCASSERT(false, "Can't find signature for VkManager::requestUserInfo");
               return;
            }   
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        
        virtual void share(const std::string &text, const std::string &linkTitle, const std::string &link, ShareCompletion completion) override {
            shareCompletionCallback = completion;

            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "share", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
               CCASSERT(false, "Can't find signature for VkManager::share");
               return;
            }   
            jstring jtext = methodInfo.env->NewStringUTF(text.c_str());
            jstring jlinkTitle = methodInfo.env->NewStringUTF(linkTitle.c_str());
            jstring jlink = methodInfo.env->NewStringUTF(link.c_str());
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jtext, jlinkTitle, jlink);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jtext);
            methodInfo.env->DeleteLocalRef(jlinkTitle);
            methodInfo.env->DeleteLocalRef(jlink);
        }    
    };

    Vkontakte *Vkontakte::getInstance() {
        static Vkontakte *instance = new AndroidVkontakte;
        return instance;
    }
}


#ifdef __cplusplus
extern "C" {
#endif

    void Java_com_planemo_libs_social_VkManager_00024Listener_onUserInfoCallback(JNIEnv *env, jobject thiz, 
        jstring firstName, jstring lastName, jstring avatarUrl, jstring email) {
        Social::AndroidVkontakte *vk = dynamic_cast<Social::AndroidVkontakte *>(Social::Vkontakte::getInstance());
        if (vk->userInfoCallback) {
            CCLOG("VkManager_00024Listener_onUserInfoCallback");
            auto userInfo = std::make_shared<Social::UserInfo>();
            userInfo->firstName = JniHelper::jstring2string(firstName);
            userInfo->lastName = JniHelper::jstring2string(lastName);
            userInfo->avatarUrl = JniHelper::jstring2string(avatarUrl);
            userInfo->email = JniHelper::jstring2string(email);
            vk->userInfoCallback(userInfo);
            vk->userInfoCallback = nullptr;
        }
    }


    void Java_com_planemo_libs_social_VkManager_00024Listener_onLoginCallbackFailure(JNIEnv *env, jobject thiz, jstring error) {
        Social::AndroidVkontakte *vk = dynamic_cast<Social::AndroidVkontakte *>(Social::Vkontakte::getInstance());
        if (vk->loginCallback) {
            CCLOG("VkManager_00024Listener_loginCallbackFailure");
            vk->loginCallback(Social::LoginResult::failure(JniHelper::jstring2string(error)));
            vk->loginCallback = nullptr;
        }
    }

    void Java_com_planemo_libs_social_VkManager_00024Listener_onLoginCallbackCancellation(JNIEnv *env, jobject thiz) {
        Social::AndroidVkontakte *vk = dynamic_cast<Social::AndroidVkontakte *>(Social::Vkontakte::getInstance());
        if (vk->loginCallback) {
            CCLOG("VkManager_00024Listener_loginCallbackCancellation");
            vk->loginCallback(Social::LoginResult::cancellation());
            vk->loginCallback = nullptr;
        }
    }


    void Java_com_planemo_libs_social_VkManager_00024Listener_onLoginCallbackSuccess(JNIEnv *env, jobject thiz, jobjectArray permissions) {
        Social::AndroidVkontakte *vk = dynamic_cast<Social::AndroidVkontakte *>(Social::Vkontakte::getInstance());
        if (vk->loginCallback) {
            CCLOG("VkManager_00024Listener_loginCallbackSuccess");
            std::vector<std::string> grantedPermissions = JniSupport::ArrayJavaObjectsToVectorString(permissions);
            vk->loginCallback(Social::LoginResult::success(grantedPermissions));
            vk->loginCallback = nullptr;
        }
    }

    void Java_com_planemo_libs_social_VkManager_00024Listener_onShareCompletionCallback(JNIEnv *env, jobject thiz, jboolean isSuccess) {
        Social::AndroidVkontakte *vk = dynamic_cast<Social::AndroidVkontakte *>(Social::Vkontakte::getInstance());
        if (vk->shareCompletionCallback) {
            CCLOG("VkManager_00024Listener_shareCompletionCallback ");
            vk->shareCompletionCallback((bool)isSuccess);
            vk->shareCompletionCallback = nullptr;
        }
    }

#ifdef __cplusplus
}
#endif
