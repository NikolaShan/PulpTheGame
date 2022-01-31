//
//  ContentProvider.h
//  Pulp
//
//  Created by Mikhail Shulepov on 23/07/15.
//
//

#pragma once

#include "IAPurchasesManager.h"
#include "NotificationsManager.h"
#include "Advertise.h"

// IAPs
class IAPContentProvider: public IA::PurchasesManager::ContentProvider {
public:
    static const std::string NO_ADS_REWARD;
    static const std::string SKIPS_REWARD;    
    
    virtual void provideContent(const std::string &reward, const cocos2d::Value &value) override;
    virtual bool hasReward(const std::string &reward) const override;
};

// Notifications
class NotificationHandler: public NotificationsManager::Handler {
public:
    virtual void hanleNotification(int tag, const cocos2d::ValueMap &info) override;
};
