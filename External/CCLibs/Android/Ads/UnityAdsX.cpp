#include <jni.h>
#include <memory>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "UnityAdsX.h"

using namespace Advertise;
USING_NS_CC;
static const std::string CB_CLASS_NAME = "com/planemo/libs/ads/UnityAdsManager";

class AndroidUnityAdsX: public UnityAdsX {

public:
	virtual void start(const std::string &gameId) override{
		//TODO
	};
    
    virtual bool canShowRewardedVideo() override{
    	//TODO
    	return true;
    }

    virtual void showRewardedVideo(std::function<void(bool success)> completion) override{
    	//TODO
    };
};

std::shared_ptr<UnityAdsX> UnityAdsX::getInstance() {
    static auto instance = std::dynamic_pointer_cast<UnityAdsX>(std::make_shared<AndroidUnityAdsX>());
    return instance;
}