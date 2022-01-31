#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"

#include "ShareManager.h"

USING_NS_CC;

static const std::string CB_CLASS_NAME = "com/planemo/libs/support/ShareManager";

static void ShareManagerSetValue(jobject instance, const std::string &value, const std::string &name) {
	JNIEnv *env = JniHelper::getEnv();

	JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), name.c_str(), "(Ljava/lang/String;)V")) {
        CCASSERT(false, ("Can't find signature for ShareManager::" + name).c_str());
    }

    jstring jarg = env->NewStringUTF(value.c_str());
    methodInfo.env->CallVoidMethod(instance, methodInfo.methodID, jarg);
    env->DeleteLocalRef(methodInfo.classID);
	env->DeleteLocalRef(jarg);
}

void ShareManager::doShare(Completion completion) {
	JNIEnv *env = JniHelper::getEnv();

    //Create java instance
    JniMethodInfo constructorInfo;
    JniHelper::getMethodInfo(constructorInfo, CB_CLASS_NAME.c_str(), "<init>", "()V");
    jobject instance = env->NewObject(constructorInfo.classID, constructorInfo.methodID);
    env->DeleteLocalRef(constructorInfo.classID);

	if (!text.empty()) {
		ShareManagerSetValue(instance, text, "setText");
    }

    if (!url.empty()) {
    	ShareManagerSetValue(instance, url, "setLinkUrl");
    }

    if (!imagePath.empty()) {
    	const std::string fullPath = FileUtils::getInstance()->fullPathForFilename(imagePath);
    	ShareManagerSetValue(instance, fullPath, "setImageFilePath");
    }

    //do share
    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "doShare", "()V")) {
        CCASSERT(false, "Can't find signature for ShareManager::doShare");
    }

    methodInfo.env->CallVoidMethod(instance, methodInfo.methodID);
    env->DeleteLocalRef(methodInfo.classID);
    env->DeleteLocalRef(instance);
}