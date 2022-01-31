//
//  ObjcBridgingSupport.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 02/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "ObjcBridgingSupport.h"

USING_NS_CC;

bool endsWith(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool hasRegisteredUrlScheme(const std::string &scheme) {
    std::string trimmedScheme = scheme;
        const std::string suffix = "://";
    if (endsWith(trimmedScheme, suffix)) {
        auto it = trimmedScheme.rfind(suffix);
        if (it != std::string::npos) {
            trimmedScheme.erase(it, suffix.size());
        }
    }
    
    static std::vector<std::string> registeredSchemes;

    if (registeredSchemes.empty()) {
        NSArray *registeredURLs = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleURLTypes"];
        for (NSDictionary *urlType: registeredURLs) {
            NSArray *schemes = [urlType objectForKey:@"CFBundleURLSchemes"];
            for (NSString *scheme: schemes) {
                registeredSchemes.push_back(scheme.UTF8String);
            }
        }
    }
    
    return std::find(registeredSchemes.begin(), registeredSchemes.end(), trimmedScheme) != registeredSchemes.end();
}

UIImage *getImageWithPath(const std::string &imagePath) {
    NSString *nsImagePath = NSStringFromStdString(imagePath);
    UIImage *image = [UIImage imageWithContentsOfFile:nsImagePath];
    if (!image) {
        const std::string fullPath = FileUtils::getInstance()->fullPathForFilename(imagePath);
        nsImagePath = NSStringFromStdString(fullPath);
        image = [UIImage imageWithContentsOfFile:nsImagePath];
    }
    if (!image) {
        NSLog(@"Share warning: can't load image at path: %s", imagePath.c_str());
    }
    return image;
}


NSString *NSStringFromStdString(const std::string &str) {
    return [NSString stringWithUTF8String:str.c_str()];
}

NSURL *NSURLFromStdString(const std::string &str) {
    return [NSURL URLWithString:NSStringFromStdString(str)];
}

NSArray *NSArrayFromStringVector(const std::vector<std::string> &vec) {
    NSMutableArray *array = [NSMutableArray array];
    for (auto &str: vec) {
        NSString *nsstr = NSStringFromStdString(str);
        [array addObject:nsstr];
    }
    return array;
}

std::vector<std::string> StdStringVectorFromNSArray(NSArray *array) {
    std::vector<std::string> vec;
    for (NSString *str in array) {
        vec.push_back([str cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    return vec;
}

NSDictionary *NSDictionaryFromStdMap(const std::map<std::string, std::string> &map) {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    for (auto &entry: map) {
        NSString *key = NSStringFromStdString(entry.first);
        NSString *value = NSStringFromStdString(entry.second);
        dict[key] = value;
    }
    return dict;
}

#pragma mark NSDictionary to ValueMap

static void addValueToDict(id nsKey, id nsValue, ValueMap &dict);
static void addItemToArray(id item, ValueVector &array);

static Value valueFromNSObject(id item) {
    // string
    if ([item isKindOfClass:[NSString class]]) {
        return Value([item UTF8String]);
    }
    
    // number
    if ([item isKindOfClass:[NSNumber class]]) {
        NSNumber* num = item;
        const char* numType = [num objCType];
        if(num == (void*)kCFBooleanFalse || num == (void*)kCFBooleanTrue) {
            return Value([num boolValue]);
        } else if(strcmp(numType, @encode(float)) == 0) {
            return Value([num floatValue]);
        } else if(strcmp(numType, @encode(double)) == 0) {
            return Value([num doubleValue]);
        } else {
            return Value([num intValue]);
        }
    }
    
    // map
    if ([item isKindOfClass:[NSDictionary class]]) {
        ValueMap dict;
        for (id subKey in [item allKeys]) {
            id subValue = [item objectForKey:subKey];
            addValueToDict(subKey, subValue, dict);
        }
        
        return Value(dict);
    }
    
    // vector
    if ([item isKindOfClass:[NSArray class]]) {
        ValueVector subArray;
        for (id subItem in item) {
            addItemToArray(subItem, subArray);
        }
       return Value(subArray);
    }

    NSLog(@"Can't represent NSObject with cocos2d::Value type");
    
    return Value::Null;
}

static void addItemToArray(id item, ValueVector &array) {
    array.push_back(valueFromNSObject(item));
}

static void addValueToDict(id nsKey, id nsValue, ValueMap &dict) {
    // the key must be a string
    CCASSERT([nsKey isKindOfClass: [NSString class]], "The key should be a string!");
    const std::string key = [nsKey UTF8String];
    dict[key] = valueFromNSObject(nsValue);
}

cocos2d::ValueMap ValueMapFromNSDictionary(NSDictionary *dict) {
    cocos2d::ValueMap ret;
    
    if (dict != nil) {
        for (id key in [dict allKeys]) {
            id value = [dict objectForKey:key];
            addValueToDict(key, value, ret);
        }
    }
    return ret;
}

#pragma mark ValueMap to NSDictionary

static void addObjectToNSDict(const std::string& key, const Value& value, NSMutableDictionary *dict);
static void addObjectToNSArray(const Value& value, NSMutableArray *array);
static void addObjectToNSDict(const int key, const Value& value, NSMutableDictionary *dict);

static id NSObjectFromValue(const Value &value) {
    switch (value.getType()) {
        case Value::Type::NONE:
            return [NSNull null];

        case Value::Type::BYTE:
            return [NSNumber numberWithInt:value.asByte()];

        case Value::Type::INTEGER:
            return [NSNumber numberWithInt:value.asInt()];
        
        case Value::Type::UNSIGNED:
            return [NSNumber numberWithUnsignedInt:value.asUnsignedInt()];
            
        case Value::Type::FLOAT:
            return [NSNumber numberWithFloat:value.asFloat()];

        case Value::Type::DOUBLE:
            return [NSNumber numberWithDouble:value.asDouble()];

        case Value::Type::BOOLEAN:
            return [NSNumber numberWithBool:value.asBool()];

        case Value::Type::STRING:
            return NSStringFromStdString(value.asString());

        case Value::Type::VECTOR: {
            NSMutableArray *array = [NSMutableArray array];
            const ValueVector &vector = value.asValueVector();
            for (const auto &e : vector) {
                addObjectToNSArray(e, array);
            }
            return array;
        }
            
        case Value::Type::MAP: {
            NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
            const ValueMap &map = value.asValueMap();
            for (auto iter = map.begin(); iter != map.end(); ++iter) {
                addObjectToNSDict(iter->first, iter->second, dictionary);
            }
            return dictionary;
        }
            
        case Value::Type::INT_KEY_MAP: {
            NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
            const ValueMapIntKey &map = value.asIntKeyMap();
            for (auto iter = map.begin(); iter != map.end(); ++iter) {
                addObjectToNSDict(iter->first, iter->second, dictionary);
            }
            return dictionary;
        }
    }
}

static void addObjectToNSArray(const Value& value, NSMutableArray *array) {
    id element = NSObjectFromValue(value);
    [array addObject:element];
}


static void addObjectToNSDict(const std::string& key, const Value& value, NSMutableDictionary *dict) {
    NSString *nskey = [NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding];
    id element = NSObjectFromValue(value);
    [dict setObject:element forKey:nskey];
}

static void addObjectToNSDict(const int key, const Value& value, NSMutableDictionary *dict) {
    NSNumber *nskey = @(key);
    id element = NSObjectFromValue(value);
    [dict setObject:element forKey:nskey];
}

NSDictionary *NSDictionaryFromValueMap(const cocos2d::ValueMap &valueMap) {
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionary];
    for (auto iter = valueMap.begin(); iter != valueMap.end(); ++iter) {
        addObjectToNSDict(iter->first, iter->second, nsDict);
    }
    return nsDict;
}