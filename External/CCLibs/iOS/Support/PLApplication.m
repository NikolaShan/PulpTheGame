//
//  PLApplication.m
//  squares
//
//  Created by Mihail Shulepov on 21/01/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import "PLApplication.h"

@implementation PLApplication

- (BOOL)openURL:(NSURL *)url {
    NSString *strURL = [url absoluteString];
    NSString *newUrlStr = strURL;
    if ([strURL rangeOfString:@"itunes" options:NSCaseInsensitiveSearch].location != NSNotFound) {
        if ([strURL rangeOfString:@"at=" options:NSCaseInsensitiveSearch].location != NSNotFound) {
            newUrlStr = [self url:strURL replaceArguementNamed:@"at" withValue:@"10lJDF"];
        } else if (url.query && url.query.length > 0) {
            newUrlStr = [strURL stringByAppendingString:@"&at=10lJDF"];
        } else {
            newUrlStr = [strURL stringByAppendingString:@"?at=10lJDF"];
        }
        if ([newUrlStr rangeOfString:@"ct=" options:NSCaseInsensitiveSearch].location != NSNotFound) {
            newUrlStr = [self url:newUrlStr replaceArguementNamed:@"ct" withValue:@"ios_replacement"];
        } else {
            newUrlStr = [newUrlStr stringByAppendingString:@"&ct=ios_replacement"];
        }
    }

    if (newUrlStr) {
        NSURL *newURL = [NSURL URLWithString:newUrlStr];
        if (newURL) {
            return [super openURL:newURL];
        }
    }
    return [super openURL:url];
}

- (NSString *)url:(NSString*)url replaceArguementNamed:(NSString *)name withValue:(NSString *)value {
    if (!url || !name || !value)  {
        return nil;
    }
    
    NSError *error;
    NSString *regexpPattern = [NSString stringWithFormat:@"%@=(\\w+)(?:&|$)", name];
    NSRegularExpression *regexp = [[NSRegularExpression alloc] initWithPattern:regexpPattern
                                                                       options:0
                                                                         error:&error];
    NSTextCheckingResult *match = [regexp firstMatchInString:url options:0 range:NSMakeRange(0, url.length)];
    if (match != nil) {
        NSRange range = [match rangeAtIndex:1];
        if (range.location != NSNotFound) {
            return [url stringByReplacingCharactersInRange:range withString:value];
        }
    }

    return nil;
}

@end
