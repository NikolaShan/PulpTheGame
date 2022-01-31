//
//  Facebook.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import <UIKit/UIKit.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>

#import "AppDelegateMultiplexer.h"
#import "ObjcBridgingSupport.h"

#include "Facebook.h"

#pragma mark - App Delegate Handler

@interface FacebookAppDelegate : NSObject<UIApplicationDelegate>

@end

@implementation FacebookAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[FBSDKApplicationDelegate sharedInstance] application:application
                             didFinishLaunchingWithOptions:launchOptions];
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [FBSDKAppEvents activateApp];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    return [[FBSDKApplicationDelegate sharedInstance] application:application
                                                          openURL:url
                                                sourceApplication:sourceApplication
                                                       annotation:annotation];
}

@end

#pragma mark - App Invitation Delegate

@interface FBInvitationDelegate : NSObject<FBSDKAppInviteDialogDelegate>

@property (strong, nonatomic, readonly) void(^completion)(BOOL success);
@property (strong, nonatomic, readonly) FBInvitationDelegate *strongSelf; //keep self strongly until dialog flow completes

@end

@implementation FBInvitationDelegate

- (instancetype)initWithCompletion:(void(^)(BOOL success))completion {
    self = [super init];
    if (self) {
        _completion = completion;
        _strongSelf = self;
    }
    return self;
}

- (void)appInviteDialog:(FBSDKAppInviteDialog *)appInviteDialog didCompleteWithResults:(NSDictionary *)results {
    NSLog(@"Invitation results:\n%@", [results description] ?: @"???");
    NSString *completionGesture = results[@"completionGesture"];
    BOOL successResult = YES;
    if (results == nil || [completionGesture isEqualToString:@"cancel"]) {
        successResult = NO;
    }
    self.completion(successResult);
    _completion = nil;
    _strongSelf = nil;
}

- (void)appInviteDialog:(FBSDKAppInviteDialog *)appInviteDialog didFailWithError:(NSError *)error {
    self.completion(NO);
    _completion = nil;
    _strongSelf = nil;
}

@end


#pragma mark - Share Delegate

@interface FBShareDelegate : NSObject<FBSDKSharingDelegate>

@property (nonatomic) Social::ShareCompletion completion;
@property (strong, nonatomic) FBShareDelegate *strongSelf;

@end

@implementation FBShareDelegate

- (instancetype)initWithCompletion:(Social::ShareCompletion)completion {
    self = [super init];
    if (self) {
        _completion = completion;
        _strongSelf = self;
    }
    return self;
}

- (void)sharer:(id<FBSDKSharing>)sharer didCompleteWithResults:(NSDictionary *)results {
    if (_completion) {
        _completion(true);
    }
    _strongSelf = nil;
}

- (void)sharer:(id<FBSDKSharing>)sharer didFailWithError:(NSError *)error {
    NSLog(@"FB share error: %@", error.description ?: @"undefined");

    if (_completion) {
        _completion(false);
    }
    _strongSelf = nil;
}

- (void)sharerDidCancel:(id<FBSDKSharing>)sharer {
    if (_completion) {
        _completion(false);
    }
    _strongSelf = nil;
}

@end


#pragma mark - Cpp

namespace Social {
    
    class IOSFacebook: public Facebook {
    public:
        IOSFacebook() {
            
        }
        
        virtual ~IOSFacebook() {}
        
        virtual void start() override {
            FacebookAppDelegate *fbAppDelegate = [[FacebookAppDelegate alloc] init];
            [[AppDelegateMultiplexer shared] addApplicationDelegate:fbAppDelegate];
#if COCOS2D_DEBUG == 1
            //check that vk scheme was whitelisted
            NSArray *whitelistedSchemes = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"LSApplicationQueriesSchemes"];
            NSArray *requiredSchemes = @[@"fbapi", @"fbauth2", @"fbshareextension"];
            for (NSString *scheme in requiredSchemes) {
                const BOOL hasWhithelistedVkScheme = [whitelistedSchemes containsObject:scheme];
                CCASSERT(hasWhithelistedVkScheme, "One of the schemes 'fbapi', 'fbauth2', 'fbshareextension' not whitelisted");
            }
#endif
        }
        
        virtual void loginWithReadPermissions(const std::vector<std::string> &permissions, LoginCallback callback) override {
            FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
            NSArray *nsPermissions = NSArrayFromStringVector(permissions);
            UIViewController *vc = [[UIApplication sharedApplication].delegate window].rootViewController;
            [login logInWithReadPermissions:nsPermissions fromViewController:vc handler:^(FBSDKLoginManagerLoginResult *result, NSError *error) {
                if (error) {
                    callback(LoginResult::failure([error.description cStringUsingEncoding:NSUTF8StringEncoding]));
                } else if (result.isCancelled) {
                    callback(LoginResult::cancellation());
                } else {
                    NSArray *nsGrantedPermissions = [result.grantedPermissions allObjects] ?: [NSArray array];
                    auto grantedPermissions = StdStringVectorFromNSArray(nsGrantedPermissions);
                    callback(LoginResult::success(grantedPermissions));
                }
            }];
        }
        
        virtual void logout() override {
            FBSDKLoginManager *lm = [[FBSDKLoginManager alloc] init];
            [lm logOut];
        }
        
        virtual bool isLoggedIn() const override {
            return [FBSDKAccessToken currentAccessToken] != nil;
        }
        
        virtual UserInfoPtr getUserInfo() override {
            FBSDKProfile *profile = [FBSDKProfile currentProfile];
            if (profile) {
                auto userInfo = std::make_shared<UserInfo>();
                if (profile.name) {
                    userInfo->name = [profile.name UTF8String];
                }
                if (profile.firstName) {
                    userInfo->firstName = [profile.firstName UTF8String];
                }
                if (profile.lastName) {
                    userInfo->lastName = [profile.lastName UTF8String];
                }
                NSURL *imageUrl = [profile imageURLForPictureMode:FBSDKProfilePictureModeSquare size:CGSizeMake(300, 300)];
                if (imageUrl) {
                    userInfo->avatarUrl = [[imageUrl absoluteString] UTF8String];
                }
                
                return userInfo;
            }
            
            return nullptr;
        }
        
        virtual bool shareLink(const std::string &link, const std::string &title, const std::string &imageUrl, const std::string &contentDescription, ShareCompletion completion) override {
            FBSDKShareDialog *dialog = [[FBSDKShareDialog alloc] init];
            FBSDKShareLinkContent *content = [[FBSDKShareLinkContent alloc] init];
            content.contentURL = [NSURL URLWithString:NSStringFromStdString(link)];
            //content.contentTitle = NSStringFromStdString(title);
            //if (!imageUrl.empty()) {
            //    content.imageURL = [NSURL URLWithString:NSStringFromStdString(imageUrl)];
            //}
            //content.contentDescription = NSStringFromStdString(contentDescription);
            
            if ([dialog canShow]) {
                UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
                FBShareDelegate *shareDelegate = [[FBShareDelegate alloc] initWithCompletion:completion];
                
                dialog.fromViewController = rootVC;
                dialog.shareContent = content;
                dialog.mode = FBSDKShareDialogModeAutomatic;
                dialog.delegate = shareDelegate;
                [dialog show];
                return true;
            } else {
                if (completion) {
                    completion(false);
                }
                return false;
            }
        }
        
        virtual bool canInviteFriends() const override {
            return [[[FBSDKAppInviteDialog alloc] init] canShow];
        }
        
        virtual bool inviteFriends(const std::string &appLinkUrl, InvitationCallback callback) override {
            if (canInviteFriends()) {
                FBInvitationDelegate *invitationDelegate = [[FBInvitationDelegate alloc] initWithCompletion:^(BOOL success) {
                    callback(success);
                }];
                FBSDKAppInviteContent *content = [[FBSDKAppInviteContent alloc] init];
                content.appLinkURL = [NSURL URLWithString:NSStringFromStdString(appLinkUrl)];
                UIViewController *vc = [[UIApplication sharedApplication].delegate window].rootViewController;
                [FBSDKAppInviteDialog showFromViewController:vc withContent:content delegate:invitationDelegate];
                return true;
            } else {
                callback(false);
                return false;
            }
        }
    };
    
    Facebook *Facebook::getInstance() {
        static Facebook *instance = new IOSFacebook;
        return instance;
    }
    
}
