//
//  AppShareActivities.m
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import "AppShareActivities.h"

@implementation AppShareActivities

+ (instancetype)shared {
    static AppShareActivities *instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[AppShareActivities alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.activities = [NSArray array];
    }
    return self;
}

- (void)addActivity:(UIActivity *)activity {
    self.activities = [self.activities arrayByAddingObject:activity];
}

@end
