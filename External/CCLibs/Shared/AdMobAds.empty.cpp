//
//  AdMobAds.empty.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#include "AdMobAds.h"

Advertise::AdMobInterstitial::AdMobInterstitial(const std::string &identifier) { }

void Advertise::AdMobInterstitial::load() { }

void Advertise::AdMobInterstitial::present() { }

Advertise::Status Advertise::AdMobInterstitial::getCurrentStatus() const { return Advertise::Status::NONE; }

Advertise::AdMobBanner::AdMobBanner(const std::string &identifier) { }

Advertise::AdMobBanner::~AdMobBanner() { }

void Advertise::AdMobBanner::present(float xPercent, float yPercent, float yAnchor) { }

void Advertise::AdMobBanner::remove() { }
