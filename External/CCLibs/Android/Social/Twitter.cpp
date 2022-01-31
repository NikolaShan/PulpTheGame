//
//  Twitter.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 17.11.15.
//
//
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"
#include "../JniSupport.h"

#include "Twitter.h"

USING_NS_CC;
static const std::string CLASS_NAME = "com/planemo/libs/social/TwitterManager";

class AndroidTwitter: public Social::TwitterManager {
private:
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for VkkManager::getInstance");
        }
        jobject jManager = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jManager;
    }

public:
    std::function<void(bool success)> completion;

    virtual void tweet(const TweetData &tweetData, std::function<void(bool success)> pcompletion) override {
        completion = pcompletion;
        const std::map<std::string, std::string> tweedDataMap = {
            {"text", tweetData.text},
            {"url", tweetData.url},
            {"imagePath", tweetData.imagePath}
        };

        jobject jTweedData = JniSupport::StdStringMapToJavaMap(tweedDataMap);
        
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CLASS_NAME.c_str(), "tweet", "(Ljava/util/Map;)V")) {
            CCASSERT(false, "Can't find signature for TwitterManager::tweet");
            return;
        }   
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jTweedData);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jTweedData);
    }

    virtual bool canTweet() const override {
        return true;
    }
};

namespace Social {
    TwitterManager *TwitterManager::getInstance() {
        static TwitterManager *instance = new AndroidTwitter;
        return instance;
    }
}


#ifdef __cplusplus
extern "C" {
#endif

void Java_com_planemo_libs_social_TwitterManager_00024Listener_onTweetResult(JNIEnv *env, jobject thiz, jboolean success) {
    AndroidTwitter *twitter = dynamic_cast<AndroidTwitter *>(Social::TwitterManager::getInstance());
    if (twitter->completion) {
        twitter->completion((bool)success);
        twitter->completion = nullptr;
    }
}


#ifdef __cplusplus
}
#endif