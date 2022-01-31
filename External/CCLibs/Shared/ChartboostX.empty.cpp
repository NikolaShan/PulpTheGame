//
//  ChartboostX.empty.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#include "ChartboostX.h"

namespace Advertise {
    class ChartboostX::Implementation {
    public:
        ChartboostX::Delegate *delegate;
    };
    
    ChartboostX::ChartboostX() {
        pimpl = ImplPtr(new Implementation);
    }
    
    ChartboostX::~ChartboostX() { }
    
    void ChartboostX::startSession(const std::string &appId, const std::string &appSignature) { }
    
    void ChartboostX::setDelegate(ChartboostX::Delegate *delegate) {
        pimpl->delegate = delegate;
    }
    
    ChartboostX::Delegate *ChartboostX::getDelegate() const { return pimpl->delegate; }
    
    bool ChartboostX::hasInterstitial(const Location &location) const {
        return false;
    }
    void ChartboostX::loadInterstitial(const Location &location) { }
    void ChartboostX::presentInterstitial(const Location &location) { }
    bool ChartboostX::hasRewardedVideo(const Location &location) const {
        return false;
    }
    void ChartboostX::loadRewardedVideo(const Location &location) { }
    void ChartboostX::presentRewardedVideo(const Location &location) { }
}
