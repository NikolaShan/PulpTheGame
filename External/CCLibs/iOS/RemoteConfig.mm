//
//  FirebaseSettings.m
//  ColorTilt
//
//  Created by Mikhail Shulepov on 22/11/2016.
//
//

#import <Foundation/Foundation.h>
#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>
#import "ObjcBridgingSupport.h"
#include "Utils/RemoteConfig.h"


class RemoteConfig::Implementation {
private:
    FIRRemoteConfig *firConfig;
    
public:
    Implementation() {
#if COCOS2D_DEBUG == 1
        BOOL devMode = YES;
#else
        BOOL devMode = NO;
#endif
        firConfig = [FIRRemoteConfig remoteConfig];
        FIRRemoteConfigSettings *remoteConfigSettings = [[FIRRemoteConfigSettings alloc] initWithDeveloperModeEnabled:devMode];
        firConfig.configSettings = remoteConfigSettings;
    }
    
    void fetch(float expirationDurationInSeconds) {
        [firConfig fetchWithExpirationDuration:expirationDurationInSeconds completionHandler:^(FIRRemoteConfigFetchStatus status, NSError * _Nullable error) {
            if (status == FIRRemoteConfigFetchStatusSuccess) {
                NSLog(@"Config fetched!");
                [firConfig activateFetched];
            } else {
                NSLog(@"Config not fetched");
                NSLog(@"Error %@", error.description);
            }
        }];
    }
    
    void setDefaults(const cocos2d::ValueMap &defaults) {
        NSDictionary *nsDefaults = NSDictionaryFromValueMap(defaults);
        [firConfig setDefaults:nsDefaults];
    }
    
    std::string getStringValue(const std::string &key) const {
        NSString *nsKey = NSStringFromStdString(key);
        return [firConfig[nsKey].stringValue cStringUsingEncoding:NSUTF8StringEncoding];
    }
    
    float getFloatValue(const std::string &key) const {
        NSString *nsKey = NSStringFromStdString(key);
        return [firConfig[nsKey].numberValue floatValue];
    }
    
    int getIntValue(const std::string &key) const {
        NSString *nsKey = NSStringFromStdString(key);
        return [firConfig[nsKey].numberValue intValue];
    }
    
    bool getBoolValue(const std::string &key) const {
        NSString *nsKey = NSStringFromStdString(key);
        return [firConfig[nsKey].numberValue boolValue];
    }
};

RemoteConfig::RemoteConfig() {
    pimpl = ImplPtr(new Implementation());
}

void RemoteConfig::fetch(float expirationDurationInSeconds) {
    pimpl->fetch(expirationDurationInSeconds);
}

void RemoteConfig::setDefaults(const cocos2d::ValueMap &defaults) {
    pimpl->setDefaults(defaults);
}

std::string RemoteConfig::getStringValue(const std::string &key) const {
    return pimpl->getStringValue(key);
}

float RemoteConfig::getFloatValue(const std::string &key) const {
    return pimpl->getFloatValue(key);
}

int RemoteConfig::getIntValue(const std::string &key) const {
    return pimpl->getIntValue(key);
}

bool RemoteConfig::getBoolValue(const std::string &key) const {
    return pimpl->getBoolValue(key);
}
    //getDataValue(const std::string &key) const;
