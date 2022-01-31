//
//  IAPStore.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#ifndef __CCLibs__IAStore__
#define __CCLibs__IAStore__

#include "base/ccMacros.h"
#include <string>
#include <vector>
#include "IATypes.h"

namespace IA {
    // JUST FOR INTERNAL USE
    class Store {
    public:
        class Delegate;
        virtual ~Store() {}
        
        static std::shared_ptr<Store> create(Delegate *delegate);
        
        virtual void requestProducts(const std::vector<std::string> &products, const std::vector<ProductType> &types) = 0;

        //Product type here required for android implementation
        virtual void buyProduct(const std::string &identifier) = 0;
        virtual bool isPurchasesEnabled() const = 0;
        virtual void restorePurchasedProducts() = 0;
        
        //returns true for iOS, false for Android (hide restore purchases button)
        virtual bool canRestorePurchases() const = 0;
        
        virtual std::string getLocalizedTitle(const std::string &productID) const = 0;
        virtual std::string getLocalizedDescription(const std::string &productID) const = 0;
        virtual std::string getProductPrice(const std::string &productID) const = 0;
        virtual double getProductPriceValue(const std::string &productID) const = 0;
               
    protected:
        Store(Delegate *pDelegate): delegate(pDelegate) {}
        
        Delegate *delegate;
        
    public:
        class Delegate {
        public:
            virtual ~Delegate() {}
            virtual void productsLoaded(const std::vector<std::string> &products) = 0;
            virtual void productsLoadFailed() = 0;
            
            virtual void productPurchaseFailed(const std::string &productID, const std::string &error) = 0;
            virtual void productPurchased(const std::string &productID) = 0;
            virtual void productPurchaseCancelled(const std::string &productID) = 0;
            
            virtual void productsRestoreFinished(const std::vector<std::string> &restoredProductIds) = 0;
            virtual void productsRestoreFailed(const std::string &error) = 0;
        };
    };
}

#endif /* defined(__CCLibs__IAStore__) */
