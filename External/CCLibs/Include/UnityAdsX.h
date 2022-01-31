//
//  UnityAdsX.h
//  Pulp
//
//  Created by Mikhail Shulepov on 01/08/15.
//
//

#pragma once

#include <string>
#include <assert.h>

#include "Advertise.h"

namespace Advertise {
    
    class UnityAdsX: public AdSystem {
    public:
        static std::shared_ptr<UnityAdsX> getInstance();
        virtual void start(const std::string &gameId) = 0;
        
        virtual bool canShowRewardedVideo() = 0;
        virtual void showRewardedVideo(std::function<void(bool success)> completion) = 0;
        
    protected:
        UnityAdsX() { }

    };
    
}