//
//  CrossPromoKit.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 18/06/15.
//
//

#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>
#import "ObjcBridgingSupport.h"

#include "CrossPromoKit.h"
#include "Utils/AppUtils.h"
#include "UI/UIProgressHUD.h"

USING_NS_CC;

@interface StoreViewControllerCloser: NSObject<SKStoreProductViewControllerDelegate>

@end

@implementation StoreViewControllerCloser

+ (instancetype)shared {
    static StoreViewControllerCloser *instance;
    if (instance == nil) {
        instance = [[StoreViewControllerCloser alloc] init];
    }
    return instance;
}

- (void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController {
    [viewController dismissViewControllerAnimated:YES completion:nil];
}

@end

void CrossPromoKit::showInAppStore(const std::shared_ptr<AppInfo> app) {
    SKStoreProductViewController *storeVC = [[SKStoreProductViewController alloc] init];
    storeVC.delegate = [StoreViewControllerCloser shared];
    
    NSMutableDictionary *params = [NSMutableDictionary dictionary];
    auto appId = app->getValueForKey(AppInfo::KEY_APP_ID).asString();
    params[SKStoreProductParameterITunesItemIdentifier] = [NSString stringWithUTF8String:appId.c_str()];
    if (&SKStoreProductParameterCampaignToken != NULL) {
        params[SKStoreProductParameterCampaignToken] = [[NSBundle mainBundle] bundleIdentifier];
    }
    
    if (&SKStoreProductParameterAffiliateToken != NULL && !affilateToken.empty()) {
        params[SKStoreProductParameterAffiliateToken] = [NSString stringWithUTF8String:affilateToken.c_str()];
    }
    
    auto providerToken = app->getValueForKey(AppInfo::KEY_PROVIDER_TOKEN);
    if (&SKStoreProductParameterProviderToken != NULL && !providerToken.isNull()) {
        params[SKStoreProductParameterProviderToken] = [NSString stringWithUTF8String:providerToken.asString().c_str()];
    }
    
    auto progressHUD = ProgressHUD::create();
    progressHUD->retain();
    progressHUD->showLoadingWithStatus("");
    [storeVC loadProductWithParameters:params completionBlock:^(BOOL result, NSError *error) {
        progressHUD->dismiss();
        progressHUD->release();
        UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
        [rootVC presentViewController:storeVC animated:YES completion:nil];
    }];
}

bool CrossPromoKit::AppInfo::isInstalled() const {
    auto it = info.find(KEY_URL_SCHEMES);
    ValueVector urlSchemes;
    if (it != info.end()) {
        switch ((*it).second.getType()) {
            case Value::Type::VECTOR:
                urlSchemes = (*it).second.asValueVector();
                break;
            default:
                urlSchemes.push_back((*it).second);
                break;
        }
    }

    for (auto schemeValue: urlSchemes) {
        //check if it self
        const std::string scheme = schemeValue.asString();
        if (hasRegisteredUrlScheme(scheme)) {
            return true;
        }
        
        if (AppUtils::isInstalledAppWithScheme(scheme)) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> CrossPromoKit::AppInfo::getRequiredFields() const {
    return {KEY_ENTRY_ID, KEY_NAME, KEY_DESCRIPTION, KEY_APP_ID, KEY_URL_SCHEMES};
}