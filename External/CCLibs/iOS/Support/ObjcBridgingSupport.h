//
//  ObjcBridgingSupport.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 02/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __CocosFrameworksBinding__ObjcBridgingSupport__
#define __CocosFrameworksBinding__ObjcBridgingSupport__

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include <string>
#include <vector>
#include <map>
#include "base/CCValue.h"

extern UIImage *getImageWithPath(const std::string &imagePath);

extern NSString *NSStringFromStdString(const std::string &str);
extern NSURL *NSURLFromStdString(const std::string &str);
extern NSArray *NSArrayFromStringVector(const std::vector<std::string> &vec);
extern std::vector<std::string> StdStringVectorFromNSArray(NSArray *array);

extern NSDictionary *NSDictionaryFromStdMap(const std::map<std::string, std::string> &map);

extern cocos2d::ValueMap ValueMapFromNSDictionary(NSDictionary *dict);
extern NSDictionary *NSDictionaryFromValueMap(const cocos2d::ValueMap &valueMap);

extern bool hasRegisteredUrlScheme(const std::string &scheme);

#endif /* defined(__CocosFrameworksBinding__ObjcBridgingSupport__) */
