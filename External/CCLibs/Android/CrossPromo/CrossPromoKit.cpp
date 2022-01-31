
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "platform/CCApplication.h"
#include "../JniSupport.h"
#include "Utils/StringUtils.h"

#include "CrossPromoKit.h"
#include "Utils/AppUtils.h"

void CrossPromoKit::showInAppStore(const std::shared_ptr<AppInfo> app) {
	auto androidStoreUrl = app->getValueForKey(AppInfo::KEY_ANDROID_STORE_URL);
	
	if (!androidStoreUrl.isNull()) {
		std::string strURL = androidStoreUrl.asString();
		::StringUtils::replaceAll(strURL, "app_name", AppUtils::getBundleIdentifier());		
		cocos2d::Application::getInstance()->openURL(strURL);
	} else {
		cocos2d::log("CrossPromoKit: undefined url for play google");
	}
}

bool CrossPromoKit::AppInfo::isInstalled() const {
	auto packageName = getValueForKey(KEY_PACKAGE_NAME);
	if (!packageName.isNull()) {
    	return AppUtils::isInstalledAppWithScheme(packageName.asString());
	}
	cocos2d::log("CrossPromoKit: %s not provided for app %s", AppInfo::KEY_PACKAGE_NAME.c_str(), getEntryIdentifier().c_str());
	return false;
}

std::vector<std::string> CrossPromoKit::AppInfo::getRequiredFields() const {
    return {KEY_ENTRY_ID, KEY_NAME, KEY_DESCRIPTION, KEY_PACKAGE_NAME, KEY_ANDROID_STORE_URL};
}