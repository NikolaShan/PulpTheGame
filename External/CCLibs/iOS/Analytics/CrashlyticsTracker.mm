//
//  CrashlyticsLogger.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 14.12.15.
//
//

#import <Crashlytics/Crashlytics.h>
#import "ObjcBridgingSupport.h"

#include "CrashlyticsTracker.h"


void CrashlyticsTracker::setString(const std::string &key, const std::string &value) {
    [[Crashlytics sharedInstance] setObjectValue:NSStringFromStdString(value) forKey:NSStringFromStdString(key)];
}

void CrashlyticsTracker::setInt(const std::string &key, const int value) {
    [[Crashlytics sharedInstance] setIntValue:value forKey:NSStringFromStdString(key)];
}

void CrashlyticsTracker::setFloat(const std::string &key, const float value) {
    [[Crashlytics sharedInstance] setFloatValue:value forKey:NSStringFromStdString(key)];
}

void CrashlyticsTracker::setBool(const std::string &key, const bool value) {
    [[Crashlytics sharedInstance] setBoolValue:value forKey:NSStringFromStdString(key)];
}

void CrashlyticsTracker::log(const std::string format, fmt::ArgList args) {
    auto result = fmt::format(format, args);
    CLSLog(@"%s", result.c_str());
}