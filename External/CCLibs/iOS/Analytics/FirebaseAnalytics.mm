//
//  FirebaseAnalytics.m
//  ColorTilt
//
//  Created by Mikhail Shulepov on 23/09/2016.
//
//

#import <Foundation/Foundation.h>
#import "FirebaseAnalytics.h"
#import <Firebase/Firebase.h>
#import "ObjcBridgingSupport.h"

class FirebaseAnalyticsIOS: public FirebaseAnalytics {
public:
    FirebaseAnalyticsIOS() {
        
    }
    
    virtual void start() override {
        [FIRApp configure];
    }
    
    virtual void sendEvent(const std::string &name,
                           const std::map<std::string, std::string> &parameters)override {
        NSDictionary *nsparameters = NSDictionaryFromStdMap(parameters);
        [FIRAnalytics logEventWithName:NSStringFromStdString(name)
                            parameters:nsparameters];
    }
};

std::shared_ptr<FirebaseAnalytics> FirebaseAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<FirebaseAnalytics>(std::make_shared<FirebaseAnalyticsIOS>());
    return instance;
}
