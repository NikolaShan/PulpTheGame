//
//  ShareManager.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 10/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppShareActivities.h"

#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"

#include "ShareManager.h"
#include "ObjcBridgingSupport.h"

USING_NS_CC;

void ShareManager::doShare(Completion completion) {
    NSMutableArray *shareItems = [NSMutableArray array];
    if (!text.empty()) {
        [shareItems addObject:NSStringFromStdString(text)];
    }
    
    if (!url.empty()) {
        NSString *urlString = NSStringFromStdString(url);
        NSURL *url = [NSURL URLWithString:urlString];
        if (url) {
            [shareItems addObject:url];
        } else {
            NSLog(@"Share warning: can't create proper url from: %@", urlString);
        }
    }
    
    if (!imagePath.empty()) {
        UIImage *image = getImageWithPath(imagePath);
        if (image) {
            [shareItems addObject:image];
        }
    }
    
    if ([shareItems count] == 0) {
        NSLog(@"Share error: no item to share were provided");
        return;
    }

    UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
    
    NSArray *activities = [[AppShareActivities shared] activities];
    UIActivityViewController *activityVC = [[UIActivityViewController alloc] initWithActivityItems:shareItems applicationActivities:activities];
    activityVC.excludedActivityTypes = @[UIActivityTypeAssignToContact, UIActivityTypeAddToReadingList, UIActivityTypePrint,UIActivityTypeMail, UIActivityTypeSaveToCameraRoll];
    if ([activityVC respondsToSelector:@selector(popoverPresentationController)]) {
        UIPopoverPresentationController *popover = [activityVC popoverPresentationController];
        if (popover) {
            const CGSize viewSize = rootVC.view.bounds.size;
            const cocos2d::Size glWinSize = cocos2d::Director::getInstance()->getWinSize();
            const float xScale = viewSize.width / glWinSize.width;
            const float yScale = viewSize.height / glWinSize.height;
            cocos2d::Rect rect;
            rect.origin.x = glRect.origin.x * xScale;
            rect.size.width = glRect.size.width * xScale;
            
            rect.origin.y = (glWinSize.height - glRect.getMaxY()) * yScale;
            rect.size.height = glRect.size.height * yScale;
            
            popover.sourceView = rootVC.view;
            popover.sourceRect = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        }
    }
    
    if (completion) {
        if ([activityVC respondsToSelector:@selector(completionWithItemsHandler)]) {
            activityVC.completionWithItemsHandler = ^(NSString *activityType, BOOL completed, NSArray *returnedItems, NSError *activityError) {
                const std::string sharedTo = activityType ? [activityType UTF8String] : "unknown";
                completion(completed, sharedTo);
            };
        } else {
            activityVC.completionHandler = ^(NSString * __nullable activityType, BOOL completed) {
                const std::string sharedTo = activityType ? [activityType UTF8String] : "unknown";
                completion(completed, sharedTo);
            };
        }
    }

    [rootVC presentViewController:activityVC animated:YES completion:nil];
}
