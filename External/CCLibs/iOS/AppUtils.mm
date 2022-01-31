//
//  Utils.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 18/06/15.
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <StoreKit/StoreKit.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "Utils/AppUtils.h"

namespace AppUtils {
    NSString *platform() {
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = (char *) malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        NSString *platform = [NSString stringWithUTF8String:machine];
        free(machine);
        return platform;
    }
    
    
    std::string getBundleIdentifier() {
        NSString *identifier = [NSBundle mainBundle].bundleIdentifier;
        return [identifier UTF8String];
    }
    
    bool isInstalledAppWithScheme(const std::string &scheme) {
        NSString *nsScheme = [NSString stringWithUTF8String:scheme.c_str()];
        NSURL *url = [NSURL URLWithString:nsScheme];
        if (url) {
            return [[UIApplication sharedApplication] canOpenURL:url];
        }
        NSLog(@"Utils: invalid URL scheme: %@", nsScheme);
        return false;
    }
    
    std::string getPreferredLanguage() {
        NSString *preferredLanguage = [[NSLocale preferredLanguages] firstObject];
        return [[preferredLanguage lowercaseString] UTF8String];
    }
    
    bool isTablet() {
        return [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad;
    }
    
    bool isIPhoneX() {
        if (!isTablet()) {
            if ([[UIScreen mainScreen] nativeBounds].size.height >= 2435.0f) {
                return YES;
            }
        }
        return NO;
    }
    
    bool isSlowDevice() {
        NSString *platformName = platform();
        NSArray *slowDevices = @[@"iPhone3,1", @"iPhone3,2", @"iPhone3,3", @"iPod3,1", @"iPad2,1", @"iPad2,2", @"iPad2,3"];
        return [slowDevices containsObject:platformName];
    }
    
    cocos2d::Size getScreenSizeInDpi() {
        CGSize size = [UIScreen mainScreen].bounds.size;
        return cocos2d::Size(MIN(size.width, size.height), MAX(size.width, size.height));
    }
    
    std::string getVersionName() {
        return [[[NSBundle mainBundle] objectForInfoDictionaryKey: @"CFBundleShortVersionString"] UTF8String];
    }
    
    int getBuildNumber() {
        NSString *build = [[NSBundle mainBundle] objectForInfoDictionaryKey: (NSString *)kCFBundleVersionKey];
        return [build intValue];
    }
    
    bool backgroundMusicShouldBeSilenced() {
        AVAudioSession *session = [AVAudioSession sharedInstance];
        if ([session respondsToSelector:@selector(secondaryAudioShouldBeSilencedHint)]) {
            return [session secondaryAudioShouldBeSilencedHint];
        } else {
            return [session isOtherAudioPlaying];
        }
    }
    
    std::string getReferrer() {
        return "";
    }
    
    bool requestSystemReview() {
        if ([[SKStoreReviewController class] respondsToSelector:@selector(requestReview)]) {
            [SKStoreReviewController requestReview];
            return true;
        }
        return false;
    }
}
