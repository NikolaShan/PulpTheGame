//
//  GameKitHelper.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>

#include <functional>
#include <memory>

typedef void(^AuthCallback)(BOOL);

@interface GameKitHelper : NSObject <GKGameCenterControllerDelegate>

+ (instancetype)helper;

@property (nonatomic, readonly) BOOL isAuthenticated;
@property (nonatomic) BOOL showCompletionBanner; //YES by default
@property (strong, nonatomic) AuthCallback globalAuthCallback;

- (void)authenticateLocalPlayer:(AuthCallback)callback;

// Scores
- (void)submitScore:(int64_t)score leaderboard:(NSString *)leaderboard completion:(void(^)(NSError *))completion;
- (void)retrieveLocalPlayerScoreForLeaderboard:(NSString *)leaderboard result:(void(^)(int highscore))result;

// Achivements
- (void)reportAchievements:(NSArray *)identifier percentages:(NSArray *)percentage completion:(void(^)(NSError *))completion;

// Game Center Views
- (void)showLeaderBoard:(NSString *)leaderboard;
- (void)showAchievements;

@end
