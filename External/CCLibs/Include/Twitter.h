//
//  Twitter.h
//  Pulp
//
//  Created by Mikhail Shulepov on 17.11.15.
//
//

#pragma once

#include <string>
#include <functional>

namespace Social {
    
    class TwitterManager {
    public:
        struct TweetData {
            std::string text;
            std::string url;
            std::string imagePath;
        };
        
        struct AppCard {
            std::string text;
            std::string imagePath; //user generated image, at least 800x320
            std::string itunesAppId;
            std::string googlePlayAppId;
        };
        
        static TwitterManager *getInstance();
        
        virtual void tweet(const TweetData &tweetData, std::function<void(bool success)> result) = 0;
        virtual bool canTweet() const = 0;
        //virtual void postAppCard(const AppCard &appCardData, std::function<void(bool success)> completion) = 0;
    };
}