//
//  Twitter.m
//  Pulp
//
//  Created by Mikhail Shulepov on 17.11.15.
//
//
#import <UIKit/UIKit.h>
#import <Social/Social.h>
//#import <TwitterKit/TwitterKit.h>

#include "ObjcBridgingSupport.h"

#include "Twitter.h"

class IOSTwitter: public Social::TwitterManager {
public:
    virtual void tweet(const TweetData &tweetData, std::function<void(bool success)> completion) override {
        if ([SLComposeViewController isAvailableForServiceType:SLServiceTypeTwitter]) {
            SLComposeViewController *tweetSheet = [SLComposeViewController
                                                   composeViewControllerForServiceType:SLServiceTypeTwitter];
            
            if (!tweetData.text.empty()) {
                [tweetSheet setInitialText:NSStringFromStdString(tweetData.text)];
            }
            if (!tweetData.url.empty()) {
                NSString *urlStr = NSStringFromStdString(tweetData.url);
                [tweetSheet addURL: [NSURL URLWithString:urlStr]];
            }
            if (!tweetData.imagePath.empty()) {
                UIImage *image = getImageWithPath(tweetData.imagePath);
                if (image) {
                    [tweetSheet addImage:image];
                }
            }
            
            tweetSheet.completionHandler = ^(SLComposeViewControllerResult result) {
                if (completion) {
                    completion(result == SLComposeViewControllerResultDone);
                }
            };
            
            if (!tweetSheet) {
                completion(false);
                return;
            }
            
            UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
            [rootVC presentViewController:tweetSheet animated:YES completion:nil];
        } else if (completion) {
            completion(false);
        }
    }    
    
    virtual bool canTweet() const override {
        return [SLComposeViewController isAvailableForServiceType:SLServiceTypeTwitter];
    }
    
 /*   virtual void postAppCard(const AppCard &appCardData, std::function<void(bool success)> completion) override {
        TWTRSession *session = [Twitter sharedInstance].sessionStore.session;
        
        // User generated image
        UIImage *image = getImageWithPath(appCardData.imagePath);
        
        // Create the card and composer
        TWTRCardConfiguration *card = [TWTRCardConfiguration appCardConfigurationWithPromoImage:image iPhoneAppID:@"1040871937" iPadAppID:@"1040871937" googlePlayAppID:@"com.nexxstudio.dulp"];
        TWTRComposerViewController *composer = [[TWTRComposerViewController alloc] initWithUserID:session.userID cardConfiguration:card];
        
        // Optionally set yourself as the delegate
        //composer.delegate = self;
        
        // Show the view controller
        UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
        [rootVC presentViewController:composer animated:YES completion:nil];
    }*/
};

namespace Social {
    TwitterManager *TwitterManager::getInstance() {
        static TwitterManager *instance = new IOSTwitter;
        return instance;
    }
}
