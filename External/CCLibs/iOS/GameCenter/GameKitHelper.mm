//
//  GameKitHelper.m
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import "GameKitHelper.h"

@interface GameKitHelper()

@property (strong, nonatomic) UIViewController *authViewController;
@property (strong, nonatomic) AuthCallback authCallback;

@end

@implementation GameKitHelper

+ (instancetype)helper {
    static GameKitHelper *instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[GameKitHelper alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.showCompletionBanner = YES;
        [self setupAuthenticationHandler];
    }
    return self;
}

- (BOOL)isAuthenticated {
    return [GKLocalPlayer localPlayer].isAuthenticated;
}

- (void)setupAuthenticationHandler {
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    if ([localPlayer respondsToSelector:@selector(setAuthenticateHandler:)]) {
        [GKLocalPlayer localPlayer].authenticateHandler = ^(UIViewController *vc, NSError *error) {
            if (vc) {
                //don't ask to auth on start automatically, only if it will be requested
                self.authViewController = vc;
                
            } else if (error) {
                NSLog(@"Auth error: %@", error.description);
                if (self.authCallback) {
                    self.authCallback(false);
                    self.authCallback = nil;
                }
                if (self.globalAuthCallback) {
                    self.globalAuthCallback(false);
                }
            } else {
                if (self.authCallback) {
                    self.authCallback(GKLocalPlayer.localPlayer.isAuthenticated);
                    self.authCallback = nil;
                }
                if (self.globalAuthCallback) {
                    self.globalAuthCallback(GKLocalPlayer.localPlayer.isAuthenticated);
                }
            }
        };
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        [localPlayer authenticateWithCompletionHandler:^(NSError *error) {
            if (error) {
                NSLog(@"Auth error: %@", error.description);
            }
            if (self.authCallback) {
                self.authCallback(GKLocalPlayer.localPlayer.isAuthenticated);
                self.authCallback = nil;
            }
        }];
#pragma clang diagnostic pop
    }
}

#pragma mark -
#pragma mark Auth

- (void)authenticateLocalPlayer:(void(^)(BOOL))callback {
    if (self.authViewController) {
        self.authCallback = callback;
        UIViewController *rootVC = [[UIApplication sharedApplication].delegate window].rootViewController;
        [rootVC presentViewController:self.authViewController animated:YES completion:nil];
        self.authViewController = nil;
    } else {
        callback(self.isAuthenticated);
    }
}


#pragma mark -
#pragma mark Scores

- (void)submitScore:(int64_t)score leaderboard:(NSString *)leaderboard completion:(void (^)(NSError *))completion {
    if (!self.isAuthenticated) {
        return;
    }
    
    GKScore *gkScore;
    if ([GKScore instancesRespondToSelector:@selector(initWithLeaderboardIdentifier:)]) {
        gkScore = [[GKScore alloc] initWithLeaderboardIdentifier:leaderboard];
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        gkScore = [[GKScore alloc] initWithCategory:leaderboard];
#pragma clang diagnostic pop
    }
    gkScore.value = score;
    
    if ([[GKScore class] respondsToSelector:@selector(reportScores:withCompletionHandler:)]) {
        [GKScore reportScores:@[gkScore] withCompletionHandler:completion];
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        [gkScore reportScoreWithCompletionHandler:completion];
#pragma clang diagnostic pop
    }
}

- (void)retrieveLocalPlayerScoreForLeaderboard:(NSString *)leaderboard result:(void (^)(int))result {
    GKLeaderboard *leaderboardRequest = [[GKLeaderboard alloc] init];
    leaderboardRequest.identifier = leaderboard;
    leaderboardRequest.playerScope = GKLeaderboardPlayerScopeFriendsOnly;
    leaderboardRequest.timeScope = GKLeaderboardTimeScopeAllTime;
    leaderboardRequest.range = NSMakeRange(1, 1);
    [leaderboardRequest loadScoresWithCompletionHandler:^(NSArray<GKScore *> *scores, NSError *error) {
        if (error) {
            result(0);
        } else {
            result(static_cast<int>(leaderboardRequest.localPlayerScore.value));
        }
    }];
}


#pragma mark -
#pragma mark Achievements

- (void)reportAchievements:(NSArray *)identifiers percentages:(NSArray *)percentages completion:(void (^)(NSError *))completion {
    if (!self.isAuthenticated) {
        return;
    }
    
    NSMutableArray *achievements = [NSMutableArray array];
    NSUInteger minCount = MIN(identifiers.count, percentages.count);
    for (NSUInteger i =0; i < minCount; ++i) {
        NSString *identifier = identifiers[i];
        const CGFloat percentage = MIN(100, [percentages[i] floatValue]);
        
        GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
        achievement.percentComplete = percentage;
        achievement.showsCompletionBanner = self.showCompletionBanner;
        [achievements addObject:achievement];
    }
    
    if ([[GKAchievement class] respondsToSelector:@selector(reportAchievements:withCompletionHandler:)]) {
        [GKAchievement reportAchievements:achievements withCompletionHandler:completion];
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        //[achievement reportAchievementWithCompletionHandler:completion];
#pragma clang diagnostic pop
    }
}


#pragma mark -
#pragma mark UI

- (void)showLeaderBoard:(NSString *)leaderboard {
    GKGameCenterViewController *gkVC = [[GKGameCenterViewController alloc] init];
    gkVC.viewState = GKGameCenterViewControllerStateLeaderboards;
    if ([gkVC respondsToSelector:@selector(leaderboardIdentifier)]) {
        gkVC.leaderboardIdentifier = leaderboard;
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        gkVC.leaderboardCategory = leaderboard;
#pragma clang diagnostic pop
    }
    [self presentGameCenterViewController:gkVC];
}

- (void)showAchievements {
    GKGameCenterViewController *gkVC = [[GKGameCenterViewController alloc] init];
    gkVC.viewState = GKGameCenterViewControllerStateAchievements;
    [self presentGameCenterViewController:gkVC];
}

- (void)presentGameCenterViewController:(GKGameCenterViewController *)vc {
    vc.gameCenterDelegate = self;
    [[[UIApplication sharedApplication].delegate window].rootViewController presentViewController:vc animated:YES completion:nil];
}


#pragma mark -
#pragma mark GK Game Center Controller Delegate

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController {
    [gameCenterViewController dismissViewControllerAnimated:YES completion:nil];
}

@end
