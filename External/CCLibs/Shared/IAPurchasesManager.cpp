//
//  IAPurchasesManager.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#include "platform/CCFileUtils.h"

#include "IAPurchasesManager.h"
#include "private/IAStore.h"
#include "IAProduct.h"
#include "Utils/EventDispatcherHelper.h"
#include "IADefaultGuiDelegate.h"


USING_NS_CC;

namespace IA {
    const std::string PurchasesManager::PRODUCTS_LOAD_FINISHED_NOTIFICATION = "IAPProductsLoaded";
    
#pragma mark -
#pragma mark Implementation
    
    class PurchasesManager::Implementation : public Store::Delegate {
    public:
        Implementation() {
            guiDelegate = std::make_shared<DefaultGuiDelegate>();
            store = Store::create(this);
            loadStatus = ProductsLoadStatus::NONE;
        }
        virtual ~Implementation() {}
        
        void registerProducts(const std::string &bundleNamesapce, const cocos2d::ValueVector &productsInfo);
        void removeProduct(const std::string &name);
        void requestProducts();
        ProductPtr getProduct(const std::string &name) const;
        ProductPtr getProductWithIdentifier(const std::string &identifier) const;
        void buyProduct(ProductPtr product, PurchaseCompletion completion);
        void restorePurchases();
        ProductPtr createProduct(const std::string &ns, const cocos2d::ValueMap &info);
        
        virtual void productsLoaded(const std::vector<std::string> &products) override;
        virtual void productsLoadFailed() override;
        virtual void productPurchaseFailed(const std::string &productID, const std::string &error) override;
        virtual void productPurchased(const std::string &productID) override;
        virtual void productPurchaseCancelled(const std::string &productID) override;
        virtual void productsRestoreFinished(const std::vector<std::string> &restoredProductIds) override;
        virtual void productsRestoreFailed(const std::string &error) override;
        
    public:
        ProductsLoadStatus loadStatus;
        PurchasesManager::GuiDelegatePtr guiDelegate;
        PurchasesManager::ContentProviderPtr contentProvider;
        Products products;
        PurchaseCompletion purchaseCompletion;
        std::shared_ptr<Store> store;
        bool isProductsLoaded = false;
    };
    
    void PurchasesManager::Implementation::registerProducts(const std::string &bundleNamesapce, const ValueVector &productsInfo) {
        CC_ASSERT(contentProvider);
        for (auto &entry : productsInfo) {
            ProductPtr product(createProduct(bundleNamesapce, entry.asValueMap()));
            products.push_back(product);
            const auto &rewards = product->getRewardsInfo();
            for (auto &reward : rewards) {
                CC_ASSERT(contentProvider->hasReward(reward.first));
            }
        }
    }
       
    void PurchasesManager::Implementation::requestProducts() {
        if (loadStatus == ProductsLoadStatus::LOADING) {
            return;
        }
        loadStatus = ProductsLoadStatus::LOADING;
        
        std::vector<std::string> productIds;
        std::vector<ProductType> types;
        for (ProductPtr product: products) {
            productIds.push_back(product->getIdentifier());
            types.push_back(product->getType());
        }
        store->requestProducts(productIds, types);
    }
    
    ProductPtr PurchasesManager::Implementation::getProduct(const std::string &name) const {
        for (ProductPtr product : products) {
            if (product->getName() == name) {
                return product;
            }
        }
        return nullptr;
    }
    
    ProductPtr PurchasesManager::Implementation::getProductWithIdentifier(const std::string &identifier) const {
        for (ProductPtr product : products) {
            if (product->getIdentifier() == identifier) {
                return product;
            }
        }
        return nullptr;
    }
    
    void PurchasesManager::Implementation::buyProduct(ProductPtr product, PurchaseCompletion pCompletion) {
        CC_ASSERT(contentProvider != nullptr);
        if (!isProductsLoaded) {
            pCompletion(false);
            return;
        }
        if (product) {
            purchaseCompletion = pCompletion;
            if (guiDelegate) {
                guiDelegate->onStartPurchaseFlow(product);
            }
            store->buyProduct(product->getIdentifier());
        } else if (pCompletion) {
            pCompletion(false);
        }
    }
    
    void PurchasesManager::Implementation::restorePurchases() {
        if (!store->canRestorePurchases()) {
            return;
        }
        if (guiDelegate) {
            guiDelegate->onStartProductsRestoration();
        }
        store->restorePurchasedProducts();
    }
    
    void PurchasesManager::Implementation::productsLoaded(const std::vector<std::string> &loadedProducts) {
        isProductsLoaded = true;
        cocos2d::log("Did load IA products: ");
        for (auto &product: loadedProducts) {
            cocos2d::log("   %s", product.c_str());
        }
        
        auto failedProducts = std::remove_if(products.begin(), products.end(), [&loadedProducts](ProductPtr product) {
            return std::find(loadedProducts.begin(), loadedProducts.end(), product->getIdentifier()) == loadedProducts.end();
        });
        products.erase(failedProducts, products.end());
        
        loadStatus = ProductsLoadStatus::LOAD_SUCCESSFULL;
        EventDispatcherHelper::getInstance().dispatchEvent(
            PurchasesManager::PRODUCTS_LOAD_FINISHED_NOTIFICATION,
            Value(true)
        );
        EventDispatcherHelper::getInstance().removeEventListener(PRODUCTS_LOAD_FINISHED_NOTIFICATION, nullptr);
    }
    
    void PurchasesManager::Implementation::productsLoadFailed() {
        loadStatus = ProductsLoadStatus::LOAD_FAILED;
        EventDispatcherHelper::getInstance().dispatchEvent(
            PurchasesManager::PRODUCTS_LOAD_FINISHED_NOTIFICATION,
            Value(false)
        );
        EventDispatcherHelper::getInstance().removeEventListener(PRODUCTS_LOAD_FINISHED_NOTIFICATION, nullptr);
    }
    
    void PurchasesManager::Implementation::productPurchaseFailed(const std::string &productID, const std::string &error) {
        ProductPtr product = getProductWithIdentifier(productID);
        if (guiDelegate) {
            guiDelegate->onProductPurchaseFailed(product, error);
        }
        if (purchaseCompletion) {
            purchaseCompletion(false);
            purchaseCompletion = nullptr;
        }
    }
    
    void PurchasesManager::Implementation::productPurchaseCancelled(const std::string &productID) {
        if (guiDelegate) {
            guiDelegate->onProductPurchaseCancelled();
        }
        if (purchaseCompletion) {
            purchaseCompletion(false);
            purchaseCompletion = nullptr;
        }
    }
    
    void PurchasesManager::Implementation::productPurchased(const std::string &productID) {
        ProductPtr product = getProductWithIdentifier(productID);
        if (product == nullptr) {
            cocos2d::log("!!!Error: Buy product error: can't fint product with identifier: %s", productID.c_str());
            return;
        }
        const auto &rewards = product->getRewardsInfo();
        for (auto &reward : rewards) {
            contentProvider->provideContent(reward.first, reward.second);
        }
        
        if (guiDelegate) {
            guiDelegate->onProductPurchased(product);
        }
        
        if (purchaseCompletion) {
            purchaseCompletion(true);
            purchaseCompletion = nullptr;
        }
    }
    
    void PurchasesManager::Implementation::productsRestoreFinished(const std::vector<std::string> &restoredProductIds) {
        Products products;
        for (auto &productId: restoredProductIds) {
            ProductPtr product = getProductWithIdentifier(productId);
            if (product) {
                products.push_back(product);
            }
        }
        if (guiDelegate) {
            guiDelegate->onFinishProductsRestoration(products);
        }
    }
    
    void PurchasesManager::Implementation::productsRestoreFailed(const std::string &error) {
        if (guiDelegate) {
            guiDelegate->onFailProductsRestoration();
        }
    }
    
    ProductPtr PurchasesManager::Implementation::createProduct(const std::string &ns, const ValueMap &info) {
        return std::make_shared<Product>(ns, info, store);
    }
    
#pragma mark -
#pragma mark Forwarding to Implementation
    
    PurchasesManager *PurchasesManager::getInstance() {
        static PurchasesManager *instance = nullptr;
        if (!instance) {
            instance = new PurchasesManager;
        }
        return instance;
    }
    
    PurchasesManager::PurchasesManager() {
        pimpl = ImplPtr(new Implementation);
    }
    
    PurchasesManager::~PurchasesManager() {
        
    }
    
    void PurchasesManager::registerProducts(const std::string &bundleNamesapce, const std::string &fileName) {
        auto info = FileUtils::getInstance()->getValueVectorFromFile(fileName);
        registerProducts(bundleNamesapce, info);
    }
    
    void PurchasesManager::registerProducts(const std::string &bundleNamesapce, const ValueVector &productsInfo) {
        pimpl->registerProducts(bundleNamesapce, productsInfo);
    }
    
    void PurchasesManager::requestProducts() {
        pimpl->requestProducts();
    }
    
    void PurchasesManager::requestProducts(std::function<void(cocos2d::EventCustom *)> callback, void *targetPtr) {
        if (callback != nullptr && targetPtr != nullptr) {
            EventDispatcherHelper::getInstance()
                .addEventListener(PRODUCTS_LOAD_FINISHED_NOTIFICATION, callback, targetPtr);
        }
        requestProducts();
    }
    
    void PurchasesManager::removeRequestProductsCallback(void *targetPtr) {
        EventDispatcherHelper::getInstance().removeEventListenersForTarget(targetPtr);
    }
    
    ProductsLoadStatus PurchasesManager::PurchasesManager::getLoadStatus() const {
        return pimpl->loadStatus;
    }
    
    void PurchasesManager::setGuiDelegate(GuiDelegatePtr guiDelegate) {
        pimpl->guiDelegate = guiDelegate;
    }
    
    PurchasesManager::GuiDelegatePtr PurchasesManager::getGuiDelegate() const {
        return pimpl->guiDelegate;
    }
    
    void PurchasesManager::setContentProvider(ContentProviderPtr contentProvider) {
        pimpl->contentProvider = contentProvider;
    }
    
    PurchasesManager::ContentProviderPtr PurchasesManager::getContentProvider() const {
        return pimpl->contentProvider;
    }
    
    const Products &PurchasesManager::getProducts() const {
        return pimpl->products;
    }
    
    ProductPtr PurchasesManager::getProduct(const std::string &name) const {
        return pimpl->getProduct(name);
    }
    
    void PurchasesManager::buyProduct(const std::string &name, PurchaseCompletion completion) {
        auto product = getProduct(name);
        if (!product) {
            CCLOG("Warning: product doesn't exist: %s", name.c_str());
        }
        buyProduct(product, completion);
    }
    
    void PurchasesManager::buyProduct(ProductPtr product, PurchaseCompletion completion) {
        pimpl->buyProduct(product, completion);
    }
    
    void PurchasesManager::restorePurchases() {
        pimpl->restorePurchases();
    }
    
    bool PurchasesManager::canRestorePurchases() {
        return pimpl->store->canRestorePurchases();
    }
    
    bool PurchasesManager::isPurchasesEnabled() const {
        return pimpl->store->isPurchasesEnabled();
    }
    
    bool PurchasesManager::isProductsLoaded() const {
        return pimpl->isProductsLoaded;
    }
    
}