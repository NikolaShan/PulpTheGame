#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "NotificationsManager.h"
#include "Utils/JsonValueConverter.h"

USING_NS_CC;
static const std::string CB_CLASS_NAME = "com/planemo/libs/notifications/NotificationsScheduler";

class AndroidNotificationsManager: public NotificationsManager {   
private:
	std::shared_ptr<Handler> handler;

public:
    AndroidNotificationsManager(std::shared_ptr<Handler> pHandler): handler(pHandler) {
    }

    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for NotificationsScheduler::getInstance");
        }
        jobject jnotmng = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jnotmng;
    }

    virtual void scheduleLocalNotification(int tag, const std::string &message, const std::string &action, int delayInSeconds, const ValueMap &info) override {
        ValueMap infoCopy = info;
        infoCopy["tag"] = Value(tag);
    	const std::string infoStr = JsonValueConverter::toJsonString(Value(infoCopy));
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "scheduleLocalNotification", "(ILjava/lang/String;ILjava/lang/String;)V")) {
            CCASSERT(false, "Can't find signature for NotificationsScheduler::scheduleLocalNotification");
            return;
        }

        jstring jmassage = methodInfo.env->NewStringUTF(message.c_str()); 
        jstring jinfo = methodInfo.env->NewStringUTF(infoStr.c_str()); 

        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, (jint)tag, jmassage, delayInSeconds, jinfo);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jmassage);
        methodInfo.env->DeleteLocalRef(jinfo);
    }
    
    virtual void cancelLocalNotifications(const std::vector<int> &tags) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "cancelLocalNotifications", "([I)V")) {
            CCASSERT(false, "Can't find signature for NotificationsScheduler::cancelLocalNotifications");
            return;
        }

        jintArray jtags = JniSupport::VectorIntToJavaIntArray(tags);
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jtags);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    void handleNotification(const std::string &infoStr) {
    	const ValueMap info = JsonValueConverter::fromJsonString(infoStr).asValueMap();
        const int tag = info.at("tag").asInt();
    	handler->hanleNotification(tag, info);
    }

};


NotificationsManager *NotificationsManager::instance = nullptr;
NotificationsManager *NotificationsManager::create(std::shared_ptr<Handler> handler, bool registerFirstSession) {
    return new AndroidNotificationsManager(handler);
}

#ifdef __cplusplus
extern "C" {
#endif

	void Java_com_planemo_libs_notifications_NotificationsScheduler_00024Listener_onReceiveNotification(JNIEnv *env, jobject thiz, jstring info) {
	    AndroidNotificationsManager *notifManager = dynamic_cast<AndroidNotificationsManager *>(NotificationsManager::getInstance());
	    notifManager->handleNotification(JniHelper::jstring2string(info));
	}

#ifdef __cplusplus
}
#endif