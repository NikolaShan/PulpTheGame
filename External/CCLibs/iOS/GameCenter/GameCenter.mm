//
//  GameCenter.mm
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import "GameKitHelper.h"

#include "GameCenter.h"
#include "ObjcBridgingSupport.h"

namespace GameKit {
    
    class IOSGameCenter: public GameCenter {
    public:
        virtual void start() override {
            [GameKitHelper helper];//just create
            [GameKitHelper helper].globalAuthCallback = ^(BOOL authenticated) {
                this->onAuthStateChanged(authenticated);
            };
        }

        virtual void setShowCompletionBanner(bool showCompletionBanner) override {
            [GameKitHelper helper].showCompletionBanner = showCompletionBanner;
        }
               
        virtual void authenticateLocalPlayer(std::function<void(bool success)> callback) override {
            [[GameKitHelper helper] authenticateLocalPlayer:^(BOOL success) {
                if (callback) {
                    callback(success);
                }
            }];
        }
        
        virtual void signOut() override {
            //nothing to do
        }
        
        virtual bool isLocalPlayerAuthenticated() const override {
            return [GameKitHelper helper].isAuthenticated;
        }
        
        virtual void showAchievements() override {
            [[GameKitHelper helper] showAchievements];
        }
        
        virtual void impl_submitScore(int64_t s, const std::string &leaderboard) override {
            [[GameKitHelper helper] submitScore:s leaderboard:NSStringFromStdString(leaderboard) completion:^(NSError *error) {
                if (error) {
                    NSLog(@"Error score submission: %@", error.description);
                }
            }];
        }
        
        virtual void impl_reportAchievements(const std::vector<std::string> &identifiers, const std::vector<int> &stepsDone, const std::vector<int> &stepsRequired) override {
            std::vector<float> percentComplete;
            const size_t count = std::min({identifiers.size(), stepsDone.size(), stepsRequired.size()});
            for (size_t i = 0; i < count; ++i) {
                const float required = stepsRequired[i];
                const float done = stepsDone[i];
                const float percentage = 100.0f * ( (required <= 0) ? 1.0f : (done / required) );
                percentComplete.push_back(percentage);
            }
            
            NSMutableArray *nsidentifiers = [NSMutableArray array];
            for (auto &&identifier: identifiers) {
                [nsidentifiers addObject:NSStringFromStdString(identifier)];
            }
            NSMutableArray *percentages = [NSMutableArray array];
            for (float percentage: percentComplete) {
                [percentages addObject:@(percentage)];
            }
            [[GameKitHelper helper] reportAchievements:nsidentifiers
                                           percentages:percentages
                                           completion:^(NSError *error) {
                                               if (error) {
                                                   NSLog(@"Error achievements submission: %@", error.description);
                                               }
                                           }];
        }
        
        virtual void impl_showLeaderBoard(const std::string &identifier) override {
            [[GameKitHelper helper] showLeaderBoard:NSStringFromStdString(identifier)];
        }
        
        virtual void getLocalPlayerScoreForLeaderboard(const std::string &leaderboardName, std::function<void(int highscore)> result) override {
            [[GameKitHelper helper] retrieveLocalPlayerScoreForLeaderboard:NSStringFromStdString(leaderboardName)
                                                                    result:^(int highscore) {
                                                                        result(highscore);
                                                                    }];
        };
    };
    
    GameCenter *GameCenter::getInstance() {
        static GameCenter *instance = new IOSGameCenter;
        return instance;
    }
}
