//
//  AppShareActivities.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface AppShareActivities : NSObject

@property (strong, nonatomic) NSArray *activities;

+ (instancetype)shared;

- (void)addActivity:(UIActivity *)activity;


@end
