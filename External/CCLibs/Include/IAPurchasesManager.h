//
//  IAPurchasesManager.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#ifndef __CCLibs__IAPurchasesManager__
#define __CCLibs__IAPurchasesManager__

#include <vector>
#include <memory>
#include <functional>

#include "base/CCValue.h"
#include "base/CCEventCustom.h"

#include "IATypes.h"
#include "IAProduct.h"


namespace IA {   
    using ProductPtr = std::shared_ptr<Product>;
    using Products = std::vector<ProductPtr>;
}

/**
 * It's wrapper around store to simplify purchase flow & products handling
 *
 */

namespace IA {
    
    class PurchasesManager {
    public:
        static const std::string PRODUCTS_LOAD_FINISHED_NOTIFICATION;
        
        class ContentProvider;
        using ContentProviderPtr = std::shared_ptr<ContentProvider>;
        
        class GuiDelegate;
        using GuiDelegatePtr = std::shared_ptr<GuiDelegate>;
        using PurchaseCompletion = std::function<void(bool success)>;
        
        static PurchasesManager *getInstance();
        
        void registerProducts(const std::string &bundleNamespace, const std::string &fileName);
        void registerProducts(const std::string &bundleNamespace, const cocos2d::ValueVector &productsInfo);
        
        void requestProducts();
        void requestProducts(std::function<void(cocos2d::EventCustom *)> callback, void *targetPtr);
        void removeRequestProductsCallback(void *targetPtr);
        ProductsLoadStatus getLoadStatus() const;
        
        void setContentProvider(ContentProviderPtr contentProvider);
        ContentProviderPtr getContentProvider() const;
        
        void setGuiDelegate(GuiDelegatePtr guiDelegate);
        GuiDelegatePtr getGuiDelegate() const;
        
        const Products &getProducts() const;
        ProductPtr getProduct(const std::string &name) const;
        
        //use completion function to immediately consume purchased product or to update some state
        void buyProduct(const std::string &name, PurchaseCompletion completion = nullptr);
        void buyProduct(ProductPtr product, PurchaseCompletion completion = nullptr);
        
        //Use this method to decide whether you should present 'Restore Purchases' button
        bool canRestorePurchases();
        void restorePurchases();
        
        bool isPurchasesEnabled() const;
        
        bool isProductsLoaded() const;
        
    private:
        PurchasesManager();
        ~PurchasesManager();
        
        class Implementation;
        using ImplPtr = std::auto_ptr<Implementation>;
        ImplPtr pimpl;
        
    public:
        class ContentProvider {
        public:
            virtual ~ContentProvider() {}
            virtual void provideContent(const std::string &reward, const cocos2d::Value &value) = 0;
            virtual bool hasReward(const std::string &reward) const = 0;
        };
        
        class GuiDelegate {
        public:
            virtual ~GuiDelegate() {}
            
            // Show some loading view here
            virtual void onStartProductsRestoration() = 0;
            virtual void onFinishProductsRestoration(const Products &restoredProducts) = 0;
            virtual void onFailProductsRestoration() = 0;
            
            // Show some loading view here
            virtual void onStartPurchaseFlow(ProductPtr product) = 0;
            
            // Display some info about successfull purchase
            virtual void onProductPurchased(ProductPtr product) = 0;
            
            // Display some info about occurred error
            virtual void onProductPurchaseFailed(ProductPtr product, const std::string &error) = 0;
            
            // Remove loading view immediately
            virtual void onProductPurchaseCancelled() = 0;
        };
    };
    
}

#endif /* defined(__CCLibs__IAPurchasesManager__) */
