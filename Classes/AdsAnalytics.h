//
//  AdsAnalytics.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 18/04/2017.
//
//

#pragma once

#include "Advertise.h"

class AdsPresentReporter: public Advertise::AppearListener {
public:
    AdsPresentReporter(Advertise::AppearListener *inner);
    virtual void adWillShow() override;
    virtual void adWillHide(bool rewarded) override;
    
private:
    Advertise::AppearListener *inner;
};

class AdsSessionCounter {
public:
    static void onAppBecameActive();
    static void onAppResignActive();
    
private:
    friend class AdsPresentReporter;
    static int countShownInterstitial;
    static int countShownRewardedVideo;
    
    static const time_t SESSION_GAP_TRESHOLD;
    static const std::string END_TIME_KEY;
    static const std::string INTERSTITIAL_COUNTER_KEY;
    static const std::string REWARDED_VIDEO_COUNTER_KEY;
};
