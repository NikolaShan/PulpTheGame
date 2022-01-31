//
//  IAProduct.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#include "IAProduct.h"
#include "private/IAStore.h"

USING_NS_CC;

namespace IA {
    Product::Product(const std::string pNs, const ValueMap &info, std::shared_ptr<Store> pStore): store(pStore) {
        ns = pNs;
        CC_ASSERT(info.find("Name") != info.end());
        name = info.at("Name").asString();
        if (info.find("UserInfo") != info.end()) {
            userInfo = info.at("UserInfo").asValueMap();
        }
        CC_ASSERT(info.find("Rewards") != info.end());
        CC_ASSERT(info.at("Rewards").getType() == Value::Type::MAP);
        rewardsInfo = info.at("Rewards").asValueMap();
        
        if (info.find("Type") == info.end()) {
            type = ProductType::CONSUMABLE;
        } else {
            const auto typeName = info.at("Type").asString();
            if (typeName == "NonConsumable") {
                type = ProductType::NON_CONSUMABLE;
            } else if (typeName == "Consumable") {
                type = ProductType::CONSUMABLE;
            } else {
                CCASSERT(false, ("Unsupported product type: " + typeName).c_str());
            }
        }
    }
    
    Product::~Product() {
        
    }
    
    std::string Product::getName() const {
        return name;
    }
    
    std::string Product::getIdentifier() const {
        return ns + "." + name;
    }
    
    std::string Product::getLocalizedTitle() const {
        return store->getLocalizedTitle(getIdentifier());
    }
    
    std::string Product::getLocalizedDescription() const {
        return store->getLocalizedDescription(getIdentifier());
    }
    
    std::string Product::getLocalizedPrice() const {
        return store->getProductPrice(getIdentifier());
    }
    
    double Product::getPriceValue() const {
        return store->getProductPriceValue(getIdentifier());
    }
    
    cocos2d::ValueMap Product::getRewardsInfo() const {
        return rewardsInfo;
    }
    
    cocos2d::Value Product::getUserInfoValue(const std::string &key) const {
        auto it = userInfo.find(key);
        if (it != userInfo.end()) {
            return (*it).second;
        }
        return Value::Null;
    }
    
    ProductType Product::getType() const {
        return type;
    }
    
}