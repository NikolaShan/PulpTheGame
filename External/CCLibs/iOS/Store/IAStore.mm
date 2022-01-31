//
//  IAPStore.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//



#import <Foundation/Foundation.h>
#import "RMStore.h"
//#import "RMStore/Optional/RMStoreUserDefaultsPersistence.h"
//#import "RMStore/Optional/RMStoreAppReceiptVerificator.h"
//#import "RMStore/Optional/RMStoreTransactionReceiptVerificator.h"
#import "ObjcBridgingSupport.h"

#include <map>
#include "private/IAStore.h"

@interface IARMStoreObserver: NSObject<RMStoreObserver>

@property (assign, nonatomic) IA::Store::Delegate *delegate;

@end

@implementation IARMStoreObserver

- (void)storePaymentTransactionFailed:(NSNotification*)notification {
    SKPaymentTransaction *transaction = notification.rm_transaction;
    if (transaction) {
        if (transaction.payment.productIdentifier) {
            const std::string productId = [transaction.payment.productIdentifier UTF8String];
            if (transaction.error.code == SKErrorPaymentCancelled) {
                self.delegate->productPurchaseCancelled(productId);
            } else {
                NSString *desc = transaction.error.description ?: @"Undefined error";
                self.delegate->productPurchaseFailed(productId, [desc UTF8String]);
            }
        } else {
            self.delegate->productPurchaseFailed("undefined", "");
        }
        
    } else {
        self.delegate->productPurchaseCancelled("undefined");
    }
}

- (void)storePaymentTransactionFinished:(NSNotification*)notification {
    SKPaymentTransaction *transaction = notification.rm_transaction;
    const std::string productId = [transaction.payment.productIdentifier UTF8String];
    self.delegate->productPurchased(productId);
}

- (void)storeProductsRequestFailed:(NSNotification*)notification {
    self.delegate->productsLoadFailed();
}

- (void)storeProductsRequestFinished:(NSNotification*)notification {
    NSArray *products = notification.rm_products;
    std::vector<std::string> ret;
    for (SKProduct *product in products) {
        ret.push_back([product.productIdentifier UTF8String]);
    }
    self.delegate->productsLoaded(ret);
}

- (void)storeRestoreTransactionsFailed:(NSNotification*)notification {
    NSError *error = notification.rm_storeError;
    if (error.description) {
        self.delegate->productsRestoreFailed([error.description UTF8String]);
    } else {
        self.delegate->productsRestoreFailed("Undefined error");
    }
}

- (void)storeRestoreTransactionsFinished:(NSNotification*)notification {
    NSArray *restoredTransactions = notification.rm_transactions;
    std::vector<std::string> productIds;
    for (SKPaymentTransaction *transaction in restoredTransactions) {
        NSString *productId = transaction.payment.productIdentifier;
        if (productId) {
            productIds.push_back([productId UTF8String]);
        }
    }
    self.delegate->productsRestoreFinished(productIds);
}

@end


#pragma mark -
#pragma mark Implementation

class IOSStore: public IA::Store {
private:
    IARMStoreObserver *observer;
    
public:
    IOSStore(IA::Store::Delegate *delegate): IA::Store(delegate) {
        observer = [[IARMStoreObserver alloc] init];
        observer.delegate = delegate;
        [[RMStore defaultStore] addStoreObserver:observer];
    }
    virtual ~IOSStore() {
        
    }
    
private:
    SKProduct *getSKProduct(const std::string &productID) const {
        NSString *nsProductID = [NSString stringWithUTF8String:productID.c_str()];
        return [[RMStore defaultStore] productForIdentifier:nsProductID];
    }
    
public:
    virtual void requestProducts(const std::vector<std::string> &products, const std::vector<IA::ProductType> &types) override {
        NSMutableSet *productIDs = [NSMutableSet set];
        for (auto &productId : products) {
            [productIDs addObject: NSStringFromStdString(productId)];
        }
        [[RMStore defaultStore] requestProducts:productIDs];
    }
    
    //Product type here required for android implementation
    virtual void buyProduct(const std::string &identifier) override {
        NSString *nsProductID = NSStringFromStdString(identifier);
        RMStore *rmstore = [RMStore defaultStore];
        [rmstore addPayment:nsProductID];
    }
    
    virtual bool isPurchasesEnabled() const override {
        return [RMStore canMakePayments];
    }
    
    virtual void restorePurchasedProducts() override {
        [[RMStore defaultStore] restoreTransactions];
    }
    
    //returns true for iOS, false for Android (hide restore purchases button)
    virtual bool canRestorePurchases() const override {
        return true;
    }
    
    virtual std::string getLocalizedTitle(const std::string &productID) const override {
        SKProduct *product = getSKProduct(productID);
        if (product && product.localizedTitle) {
            return [product.localizedTitle UTF8String];
        }
        return "";
    }
    
    virtual std::string getLocalizedDescription(const std::string &productID) const override {
        SKProduct *product = getSKProduct(productID);
        if (product && product.localizedDescription) {
            return [product.localizedDescription UTF8String];
        }
        return "";
    }
    
    virtual std::string getProductPrice(const std::string &productID) const override {
        SKProduct *product = getSKProduct(productID);
        if (product) {
            NSString *price = [RMStore localizedPriceOfProduct:product];
            if (price) {
                return [price UTF8String];
            }
        }
        return "";
    }
    
    virtual double getProductPriceValue(const std::string &productID) const override {
        SKProduct *product = getSKProduct(productID);
        if (product) {
            return [product.price doubleValue];
        }
        return 0;
    }
};

std::shared_ptr<IA::Store> IA::Store::create(Store::Delegate *delegate) {
    return std::make_shared<IOSStore>(delegate);
}
