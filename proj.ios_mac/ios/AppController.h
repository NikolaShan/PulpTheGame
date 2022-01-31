#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate, SKStoreProductViewControllerDelegate, UIWebViewDelegate> {
}

@property(nonatomic, readonly) RootViewController* viewController;

@end

