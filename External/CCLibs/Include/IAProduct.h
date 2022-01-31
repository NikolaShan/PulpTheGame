//
//  IAProduct.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#ifndef __CCLibs__IAProduct__
#define __CCLibs__IAProduct__

#include <string>

#include "base/CCValue.h"
#include "IATypes.h"

namespace IA {
    
    class Store;
    
    /**
     * Product Identifier composed from bundleNamespace and Name;
     * It's useful when you have different app versions (pro-version, free-version, etc)
     * so in products definition you need to define only product name
     *
     * Methods getLocalizedTitle & getLocalizedMessage returns values in device localization,
     * so if you use custom localization and have ability to change language inside application - don't use these methods
     */
    class Product {
    public:
        Product(const std::string bundleNamespace, const cocos2d::ValueMap &info, std::shared_ptr<Store> store);
        virtual ~Product();
        
        std::string getName() const;
        std::string getIdentifier() const;
        
        std::string getLocalizedTitle() const;
        std::string getLocalizedDescription() const;
        std::string getLocalizedPrice() const;
        double getPriceValue() const;
        
        cocos2d::ValueMap getRewardsInfo() const;
        
        cocos2d::Value getUserInfoValue(const std::string &key) const;
        
        ProductType getType() const;
        
    private:
        cocos2d::ValueMap userInfo;
        cocos2d::ValueMap rewardsInfo;
        ProductType type;
        std::string name;
        std::string ns;
        std::shared_ptr<Store> store;
    };
    
}

#endif /* defined(__CCLibs__IAProduct__) */
