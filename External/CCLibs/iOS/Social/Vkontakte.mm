//
//  Vkontakte.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import <UIKit/UIKit.h>
#import "VKSdk.h"
#import "ObjcBridgingSupport.h"
#import "AppDelegateMultiplexer.h"
#import "AppShareActivities.h"

#include "Vkontakte.h"

static NSString *const VK_USER_EMAIL_KEY = @"EmUFrVk";

#pragma mark VK Application Delegate

@interface VkAppDelegate : NSObject<UIApplicationDelegate>

@end

@implementation VkAppDelegate

-(BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    return [VKSdk processOpenURL:url fromApplication:sourceApplication];
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {
    [VKSdk processOpenURL:url fromApplication:options[UIApplicationOpenURLOptionsSourceApplicationKey]];
    return YES;
}

@end

#pragma mark - VK Delegate

@interface VkDelegate: NSObject<VKSdkDelegate, VKSdkUIDelegate>

@property (nonatomic) Social::LoginCallback loginCallback;

@end

@implementation VkDelegate

// VkSdkDelegate

- (void)vkSdkAccessAuthorizationFinishedWithResult:(VKAuthorizationResult *)result {
    auto loginCallback = self.loginCallback;
    if (loginCallback) {
        self.loginCallback = nullptr;
        if (result.token) {
            auto grantedPermissions = StdStringVectorFromNSArray([result.token permissions]);
            loginCallback(Social::LoginResult::success(grantedPermissions));
        } else {
            if (result.error) {
                NSError *error = result.error;
                if (error.code == VK_API_CANCELED) {
                    loginCallback(Social::LoginResult::cancellation());
                } else {
                    loginCallback(Social::LoginResult::failure([error.description UTF8String]));
                }
            } else {
                loginCallback(Social::LoginResult::failure("unknown"));
            }
        }
    }
}

- (void)vkSdkUserAuthorizationFailed {
    auto loginCallback = self.loginCallback;
    if (loginCallback) {
        self.loginCallback = nullptr;
        loginCallback(Social::LoginResult::failure("unknown"));
    }
}

- (void)vkSdkAccessTokenUpdated:(VKAccessToken *)newToken oldToken:(VKAccessToken *)oldToken {
    if (newToken.email) {
        [[NSUserDefaults standardUserDefaults] setObject:newToken.email forKey:VK_USER_EMAIL_KEY];
    }
}

- (void)vkSdkTokenHasExpired:(VKAccessToken *)expiredToken {
    
}


// VkSdkUiDelegate

- (void)vkSdkShouldPresentViewController:(UIViewController *)controller {
    UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
    [rootVC presentViewController:controller animated:YES completion:nil];
}

- (void)vkSdkNeedCaptchaEnter:(VKError *)captchaError {
    VKCaptchaViewController *captchaVC = [VKCaptchaViewController captchaControllerWithError:captchaError];
    UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
    [captchaVC presentIn:rootVC];
}

@end


#pragma mark - Cpp

namespace Social {
    
    class IOSVkontakte: public Vkontakte {
    private:
        VkDelegate *vkDelegate;
        VkAppDelegate *appDelegate;
        
    public:
        IOSVkontakte() {
            vkDelegate = [[VkDelegate alloc] init];
            appDelegate = [[VkAppDelegate alloc] init];
            [[AppDelegateMultiplexer shared] addApplicationDelegate:appDelegate];
            [[AppShareActivities shared] addActivity:[[VKActivity alloc] init]];
        }
        virtual ~IOSVkontakte() {}
        
        virtual void startWithAppId(const std::string &appId) override {
            NSString *nsAppId = NSStringFromStdString(appId);
#if COCOS2D_DEBUG == 1
            //check that proper scheme was registered
            std::string vkScheme ="vk" + appId;
            const bool schemeRegistered = hasRegisteredUrlScheme(vkScheme);
            const std::string errorMsg = "URL scheme for VK not registered or doesn't match provided app id (vk" + appId + ")";
            CCASSERT(schemeRegistered, errorMsg.c_str());
            
            //check that vk scheme was whitelisted
            NSArray *whitelistedSchemes = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"LSApplicationQueriesSchemes"];
            const BOOL hasWhithelistedVkScheme = [whitelistedSchemes containsObject:@"vkauthorize"];
            CCASSERT(hasWhithelistedVkScheme, "Scheme 'vkauthorize' not whitelisted");
#endif
            [VKSdk initializeWithAppId:nsAppId];
            [[VKSdk instance] registerDelegate: vkDelegate];
            [[VKSdk instance] setUiDelegate: vkDelegate];
            [VKSdk wakeUpSession:@[] completeBlock:^(VKAuthorizationState, NSError *) {
                
            }];
        }
        
        virtual void loginWithPermissions(const std::vector<std::string> &permissions, LoginCallback callback) override {
            vkDelegate.loginCallback = callback;
            [VKSdk authorize: NSArrayFromStringVector(permissions)];
        }
        
        virtual void logout() override {
            [VKSdk forceLogout];
        }
        
        virtual bool isLoggedIn() const override {
            return [VKSdk isLoggedIn];
        }
        
        virtual void requestUserInfo(UserInfoCallback callback) override {
            if (isLoggedIn()) {
                NSDictionary *params = @{ @"user_ids": [VKSdk accessToken].userId, @"fields": @"photo_100" };
            
                VKRequest *request = [[VKApi users] get:params];
                [request executeWithResultBlock:^(VKResponse *response) {
                    NSDictionary *userData = [response.json firstObject];
                
                    UserInfoPtr info = std::make_shared<UserInfo>();
                    NSString *avatarUrl = [userData objectForKey:@"photo_100"];
                    NSString *firstName = [userData objectForKey:@"first_name"];
                    NSString *lastName = [userData objectForKey:@"last_name"];
                    NSString *email = [[NSUserDefaults standardUserDefaults] objectForKey:VK_USER_EMAIL_KEY];
                
                    if (avatarUrl) {
                        info->avatarUrl = [avatarUrl UTF8String];
                    }
                    if (firstName) {
                        info->firstName = [firstName UTF8String];
                    }
                    if (lastName) {
                        info->lastName = [lastName UTF8String];
                    }
                    if (email) {
                        info->email = [email UTF8String];
                    }
                    info->name = info->firstName + " " + info->lastName;
                
                    callback(info);
                
                } errorBlock:^(NSError *error) {
                    callback(nullptr);
                }];
            } else {
                callback(nullptr);
            }
        }
    
        virtual void share(const std::string &text, const std::string &linkTitle, const std::string &link, ShareCompletion completion) override {
            UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
            
            VKShareDialogController *shareDialog = [[VKShareDialogController alloc] init];
            shareDialog.text = NSStringFromStdString(text);
            shareDialog.shareLink = [[VKShareLink alloc] initWithTitle:NSStringFromStdString(linkTitle)
                                                                  link:NSURLFromStdString(link)];
            shareDialog.completionHandler = ^(VKShareDialogController *dialog, VKShareDialogControllerResult result) {
                [rootVC dismissViewControllerAnimated:YES completion:nil];
                if (completion) {
                    completion(result == VKShareDialogControllerResultDone);
                }
            };
            [rootVC presentViewController:shareDialog animated:YES completion:nil];
        }
    
        virtual bool isVkCountry() const override {
            NSLocale *currentLocale = [NSLocale currentLocale];
            NSString *localIdent = currentLocale.localeIdentifier;
            NSArray *vkCountries = @[@"ru", @"kz", @"az", @"ab", @"al", @"am", @"by", @"bg", @"ge", @"lt", @"ua", @"md", @"tj", @"ee"];
            for (NSString *country in vkCountries) {
                if ([localIdent rangeOfString:country options:NSCaseInsensitiveSearch].location != NSNotFound) {
                    return true;
                }
            }
            return false;
        }
    
    };

    Vkontakte *Vkontakte::getInstance() {
        static Vkontakte *instance = new IOSVkontakte;
        return instance;
    }
}
