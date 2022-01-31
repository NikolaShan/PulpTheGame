//
//  AdColonyX.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 08/07/15.
//
//

#import <AdColony/AdColony.h>
#import "ObjcBridgingSupport.h"
#import "AdColonyMultiplexerDelegate.h"

#include "AdColonyX.h"

using namespace Advertise;

class IOSAdColonyX;

@interface IOSAdColonyDelegate: NSObject<AdColonyDelegate>

@property (nonatomic) IOSAdColonyX *adColonyX;

@end

@interface IOSAdColoneAdDelegate : NSObject<AdColonyAdDelegate>

@property (nonatomic) std::function<void(bool)> completion;
@property (nonatomic) Advertise::AppearListener *appearListener;

- (instancetype)initWithCompletion:(std::function<void(bool)>)completion;

@end


class IOSAdColonyX: public AdColonyX {
private:
    IOSAdColonyDelegate *adColonyDelegate;
    
public:
    IOSAdColonyX() {
        adColonyDelegate = [[IOSAdColonyDelegate alloc] init];
        adColonyDelegate.adColonyX = this;

#if DEBUG
        NSArray *nsconts = @[
                             ADC_SET_USER_AGE,
                             ADC_SET_USER_INTERESTS,
                             ADC_SET_USER_GENDER,
                             ADC_SET_USER_LATITUDE,
                             ADC_SET_USER_LONGITUDE,
                             ADC_SET_USER_ANNUAL_HOUSEHOLD_INCOME,
                             ADC_SET_USER_MARITAL_STATUS,
                             ADC_SET_USER_EDUCATION,
                             ADC_SET_USER_ZIPCODE,
                             ADC_USER_MALE,
                             ADC_USER_FEMALE,
                             ADC_USER_SINGLE,
                             ADC_USER_MARRIED,
                             ADC_USER_EDUCATION_GRADE_SCHOOL,
                             ADC_USER_EDUCATION_SOME_HIGH_SCHOOL,
                             ADC_USER_EDUCATION_HIGH_SCHOOL_DIPLOMA,
                             ADC_USER_EDUCATION_SOME_COLLEGE,
                             ADC_USER_EDUCATION_ASSOCIATES_DEGREE,
                             ADC_USER_EDUCATION_BACHELORS_DEGREE,
                             ADC_USER_EDUCATION_GRADUATE_DEGREE,
        ];
        
        const std::vector<std::string> cppconsts = {
            KEY_USER_AGE,
            KEY_USER_INTERESTS,
            KEY_USER_GENDER,
            KEY_USER_LATITUDE,
            KEY_USER_LONGITUDE,
            KEY_USER_ANNUAL_HOUSEHOLD_INCOME,
            KEY_USER_MARITAL_STATUS,
            KEY_USER_EDUCATION,
            KEY_USER_ZIPCODE,
            USER_MALE,
            USER_FEMALE,
            USER_SINGLE,
            USER_MARRIED,
            USER_EDUCATION_GRADE_SCHOOL,
            USER_EDUCATION_SOME_HIGH_SCHOOL,
            USER_EDUCATION_HIGH_SCHOOL_DIPLOMA,
            USER_EDUCATION_SOME_COLLEGE,
            USER_EDUCATION_ASSOCIATES_DEGREE,
            USER_EDUCATION_BACHELORS_DEGREE,
            USER_EDUCATION_GRADUATE_DEGREE,
        };
        
        for (ssize_t i = 0; i < cppconsts.size(); ++i) {
            NSString *nskey = nsconts[i];
            std::string cppkey = cppconsts[i];
            CCASSERT(cppkey == [nskey UTF8String], "Mismatch in cpp defined and AdColony constants");
        }
        
#endif
    }
    
    virtual void start(const std::string &appId, const std::vector<std::string> &zoneIds, bool logEnabled) override {
        [AdColony configureWithAppID: NSStringFromStdString(appId)
                             zoneIDs: NSArrayFromStringVector(zoneIds)
                            delegate: [AdColonyMultiplexerDelegate sharedInstance]
                             logging: logEnabled];
    }
    
    virtual bool isVirtualCurrencyRewardAvailableForZone(const std::string &zoneID) override {
        return [AdColony isVirtualCurrencyRewardAvailableForZone:NSStringFromStdString(zoneID)];
    }
    
    virtual int getVirtualCurrencyRewardsAvailableTodayForZone(const std::string &zoneID) override {
        return [AdColony getVirtualCurrencyRewardsAvailableTodayForZone:NSStringFromStdString(zoneID)];
    }
    
    virtual int getVirtualCurrencyRewardAmountForZone(const std::string &zoneID) override {
        return [AdColony getVirtualCurrencyRewardAmountForZone:NSStringFromStdString(zoneID)];
    }
    
    virtual std::string getVirtualCurrencyNameForZone(const std::string &zoneID) override {
        return [[AdColony getVirtualCurrencyNameForZone:NSStringFromStdString(zoneID)] UTF8String];
    }
    
    virtual void playRewardedVideoForZone(const std::string &zoneID, std::function<void(bool)> completion) override {
        IOSAdColoneAdDelegate *adDelegate = [[IOSAdColoneAdDelegate alloc] initWithCompletion:completion];
        adDelegate.appearListener = appearListener;
        [AdColony playVideoAdForZone:NSStringFromStdString(zoneID)
                        withDelegate:adDelegate
                    withV4VCPrePopup:NO
                    andV4VCPostPopup:YES];
    }
    
    virtual void setUserMetadata(const std::string &metadataType, const std::string &value) override {
        [AdColony setUserMetadata:NSStringFromStdString(metadataType) withValue:NSStringFromStdString(value)];
    }
};

std::shared_ptr<AdColonyX> AdColonyX::getInstance() {
    static auto instance = std::dynamic_pointer_cast<AdColonyX>(std::make_shared<IOSAdColonyX>());
    return instance;
}

@implementation IOSAdColonyDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        [[AdColonyMultiplexerDelegate sharedInstance] addDelegate:self];
    }
    return self;
}

- (void)dealloc {
    [[AdColonyMultiplexerDelegate sharedInstance] removeDelegate:self];
}

- (void)onAdColonyV4VCReward:( BOOL )success currencyName:( NSString * )currencyName currencyAmount:( int )amount inZone:( NSString * )zoneID {
    NSLog(@"AdColony: reward");
}

@end

@implementation IOSAdColoneAdDelegate

- (instancetype)initWithCompletion:(std::function<void (bool)>)completion {
    self = [super init];
    if (self) {
        _completion = completion;
    }
    return self;
}

- ( void ) onAdColonyAdAttemptFinished:(BOOL)shown inZone:( NSString * )zoneID {
    NSLog(@"AdColony: completion");
    if (_completion) {
        _completion(shown);
    }
    if (_appearListener) {
        _appearListener->adWillHide(true);
    }
}

- (void)onAdColonyAdStartedInZone:(NSString *)zoneID {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

@end