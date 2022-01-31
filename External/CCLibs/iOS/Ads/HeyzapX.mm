//
//  HeyzapX.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 08/09/15.
//
//

#import <HeyzapAds/HeyzapAds.h>

#import "ObjcBridgingSupport.h"

#include <set>
#include "HeyzapX.h"

namespace Advertise {
    class IOSHeyzapX;
}

@interface IOSHeyzapIncentivezedAdDelegate: NSObject<HZIncentivizedAdDelegate>

@property (nonatomic) Advertise::IOSHeyzapX *heyzap;
@property (nonatomic) std::function<void(bool success)> viewCompletion;
@property (nonatomic) Advertise::AppearListener *appearListener;

@end


@interface IOSHeyzapInterstitialAdDelegate : NSObject<HZAdsDelegate>

@property (nonatomic) Advertise::IOSHeyzapX *heyzap;
@property (nonatomic) Advertise::AppearListener *appearListener;

@end


namespace Advertise {
    
    const std::string HeyzapX::DEFAULT_TAG = "default";
    
    class IOSHeyzapX: public HeyzapX {
    public:
        IOSHeyzapIncentivezedAdDelegate *incentivezedAdDelegate;
        IOSHeyzapInterstitialAdDelegate *interstitialAdDelegate;
        
        std::set<std::string> loadingInterstitials;
        std::set<std::string> loadingRewardedVideos;
        
    public:
        IOSHeyzapX() {
            incentivezedAdDelegate = [[IOSHeyzapIncentivezedAdDelegate alloc] init];
            incentivezedAdDelegate.heyzap = this;
            incentivezedAdDelegate.appearListener = appearListener;
            [HZIncentivizedAd setDelegate:incentivezedAdDelegate];
            
            interstitialAdDelegate = [[IOSHeyzapInterstitialAdDelegate alloc] init];
            interstitialAdDelegate.heyzap = this;
            interstitialAdDelegate.appearListener = appearListener;
            [HZInterstitialAd setDelegate:interstitialAdDelegate];
        }
        
        virtual void startSession(const std::string &publisherId) override {
            [HeyzapAds startWithPublisherID:NSStringFromStdString(publisherId) andOptions:HZAdOptionsDisableAutoPrefetching];
        }
        
        virtual void loadInterstitial(const std::string &tag) override {
            if (getInterstitialStatus(tag) != Status::NONE) {
                return;
            }
            loadingInterstitials.insert(tag);
            
            if (tag.empty()) {
                [HZInterstitialAd fetch];
            } else {
                [HZInterstitialAd fetchForTag:NSStringFromStdString(tag)];
            }
        }
        
        virtual void presentInterstitial(const std::string &tag) override {
            if (getInterstitialStatus(tag) == Status::LOADED) {
                NSString *nsTag = NSStringFromStdString(tag);
                [HZInterstitialAd showForTag:nsTag completion:^(BOOL result, NSError *error) {

                }];
            }
        }
        
        virtual Status getInterstitialStatus(const std::string &tag) override {
            bool ready = false;
            if (tag.empty()) {
                ready = [HZInterstitialAd isAvailable];
            } else {
                ready = [HZInterstitialAd isAvailableForTag:NSStringFromStdString(tag)];
            }
            if (ready) {
                return Status::LOADED;
            } else if (loadingInterstitials.find(tag) != loadingInterstitials.end()) {
                return Status::LOADING;
            } else {
                return Status::NONE;
            }
        }
        
        void didFinishLoadingInterstitialWithTag(const std::string &tag, bool success) {
            loadingInterstitials.erase(tag);
        }
        
        virtual void loadRewardedVideo(const std::string &tag, std::function<void(bool)> completion) override {
            if (getRewardedVideoStatus(tag) != Status::NONE) {
                return;
            }
            loadingRewardedVideos.insert(tag);
            
            typedef void(^FetchCompletion)(BOOL result, NSError *error);
            FetchCompletion fetchCompletion = ^(BOOL result, NSError *error) {
                if (error) {
                    NSLog(@"Heyzap rewarded video load error: %@", [error description]);
                }
                if (completion) {
                    completion(result);
                }
            };

            if (tag.empty()) {
                [HZIncentivizedAd fetchWithCompletion:fetchCompletion];
            } else {
                [HZIncentivizedAd fetchForTag:NSStringFromStdString(tag) withCompletion:fetchCompletion];
            }
        }
        
        virtual void presentRewardedVideo(const std::string &tag, std::function<void(bool success)> completion) override {
            if (getRewardedVideoStatus(tag) == Status::LOADED) {
                incentivezedAdDelegate.viewCompletion = completion;
                
                if (tag.empty()) {
                    [HZIncentivizedAd show];
                } else {
                    [HZIncentivizedAd showForTag:NSStringFromStdString(tag)];
                }
            }
        }
        
        virtual Status getRewardedVideoStatus(const std::string &tag) const override {
            bool ready = false;
            if (tag.empty()) {
                ready = [HZIncentivizedAd isAvailable];
            } else {
                ready = [HZIncentivizedAd isAvailableForTag:NSStringFromStdString(tag)];
            }
            if (ready) {
                return Status::LOADED;
            } else if (loadingRewardedVideos.find(tag) != loadingRewardedVideos.end()) {
                return Status::LOADING;
            } else {
                return Status::NONE;
            }
        }
        
        void didFinishLoadingRewardedVideoWithTag(const std::string &tag, bool success) {
            loadingRewardedVideos.erase(tag);
        }
    };
    
    HeyzapX *HeyzapX::getInstance() {
        static HeyzapX *instance = new IOSHeyzapX;
        return instance;
    }
    
#pragma mark -
#pragma mark Banner
    
    class IOSHeyzapBanner: public HeyzapBanner {
    private:
        bool isLoading = false;
        bool removeOnLoad = false;
        std::shared_ptr<bool> aliveMarker;
        HZBannerAd *bannerView;
        
    public:
        IOSHeyzapBanner(const std::string &pTag): HeyzapBanner(pTag) {
            bannerView = nil;
            aliveMarker = std::make_shared<bool>(true);
        }
        
        virtual ~IOSHeyzapBanner() {
            *aliveMarker = false;
            remove();
        }
        
        virtual void remove() override {
            if (bannerView) {
                HZBannerAd *hzBannerView = bannerView;
                bannerView = nil;
                dispatch_block_t block = ^{
                    [hzBannerView removeFromSuperview];
                };
                if ([NSThread isMainThread]) {
                    block();
                } else {
                    dispatch_async(dispatch_get_main_queue(), block);
                }
            }
            if (isLoading) {
                removeOnLoad = true;
            }
        }
        
        void presentBanner(HZBannerAd *banner, float xPercent, float yPercent, float yAnchor) {
            bannerView = banner;
            
            UIViewController *vc = [[[UIApplication sharedApplication] delegate] window].rootViewController;
            bannerView.layer.anchorPoint = CGPointMake(0.0f, 1.0f - yAnchor);
            bannerView.autoresizingMask = UIViewAutoresizingNone;
            
            CGFloat xPos = 0;
            CGFloat yPos = (1.0f - yPercent) * vc.view.frame.size.height;
            
            [bannerView setCenter:CGPointMake(xPos, yPos)];
            if (!bannerView.superview) {
                [vc.view addSubview:bannerView];
            }
        }
        
        virtual void present(float xPercent, float yPercent, float yAnchor) override {
            removeOnLoad = false;
            if (isLoading || bannerView) {
                return;
            }
            isLoading = true;
            
            HZBannerAdOptions *options = [[HZBannerAdOptions alloc] init];

            UIViewController *vc = [[[UIApplication sharedApplication] delegate] window].rootViewController;
            options.presentingViewController = vc;
            if (UIInterfaceOrientationIsPortrait([vc interfaceOrientation])) {
                options.admobBannerSize = HZAdMobBannerSizeFlexibleWidthPortrait;
            } else {
                options.admobBannerSize = HZAdMobBannerSizeFlexibleWidthLandscape;
            }
            if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
                options.facebookBannerSize = HZFacebookBannerSizeFlexibleWidthHeight90;
            } else {
                options.facebookBannerSize = HZFacebookBannerSizeFlexibleWidthHeight50;
            }
            if (!tag.empty()) {
                options.tag = NSStringFromStdString(tag);
            }
            
            std::shared_ptr<bool> isAlive = aliveMarker;
            [HZBannerAd requestBannerWithOptions:options success:^(HZBannerAd *banner) {
                if (*isAlive) {
                    if (!removeOnLoad) {
                        presentBanner(banner, xPercent, yPercent, yAnchor);
                    }
                    removeOnLoad = false;
                }
             } failure:^(NSError *error) {
                NSLog(@"Loading heyzap banner error: %@", [error description]);
                if (*isAlive) {
                    isLoading = false;
                    removeOnLoad = false;
                }
             }];
        }
    };
    
    std::shared_ptr<Banner> HeyzapBanner::create(const std::string &tag){
        return std::static_pointer_cast<Banner>(std::make_shared<IOSHeyzapBanner>(tag));
    }
}

@implementation IOSHeyzapInterstitialAdDelegate

- (void)didReceiveAdWithTag: (NSString *) tag {
    std::string stdtag = tag ? [tag UTF8String] : "";
    self.heyzap->didFinishLoadingInterstitialWithTag(stdtag, true);
}

- (void)didFailToReceiveAdWithTag: (NSString *) tag {
    NSLog(@"Fetch interstitial failure");
    std::string stdtag = tag ? [tag UTF8String] : "";
    self.heyzap->didFinishLoadingInterstitialWithTag(stdtag, false);
}

- (void)didShowAdWithTag: (NSString *) tag {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

- (void)didHideAdWithTag: (NSString *) tag {
    if (_appearListener) {
        _appearListener->adWillHide(false);
    }
}

@end

@implementation IOSHeyzapIncentivezedAdDelegate

- (void)completionFailure {
    if (_viewCompletion) {
        _viewCompletion(false);
        _viewCompletion = nullptr;
    }
}

- (void)didFailToShowAdWithTag: (NSString *) tag andError: (NSError *)error {
    [self completionFailure];
}

- (void)didReceiveAdWithTag: (NSString *) tag {
    std::string stdtag = tag ? [tag UTF8String] : "";
    self.heyzap->didFinishLoadingRewardedVideoWithTag(stdtag, true);
}

- (void)didFailToReceiveAdWithTag: (NSString *) tag {
    [self completionFailure];
    std::string stdtag = tag ? [tag UTF8String] : "";
    self.heyzap->didFinishLoadingRewardedVideoWithTag(stdtag, false);
}

- (void)didCompleteAdWithTag: (NSString *) tag {
    if (_viewCompletion) {
        _viewCompletion(true);
        _viewCompletion = nullptr;
    }
}

- (void)didFailToCompleteAdWithTag: (NSString *) tag {
    [self completionFailure];
}

- (void)didShowAdWithTag: (NSString *) tag {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

- (void)didHideAdWithTag: (NSString *) tag {
    if (_appearListener) {
        _appearListener->adWillHide(true);
    }
}

@end