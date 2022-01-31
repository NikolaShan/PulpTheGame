//
//  WeakObject.m
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import "WeakObject.h"

@implementation WeakObject

+ (instancetype)with:(id)object {
    return [[self alloc] initWithObject:object];
}

- (instancetype)initWithObject:(id)object {
    if (self = [super init]) {
        _object = object;
    }
    return self;
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)sel {
    return [self.object methodSignatureForSelector:sel];
}

- (BOOL)respondsToSelector:(SEL)aSelector {
    return [self.object respondsToSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
    if (self.object != nil) {
        [anInvocation invokeWithTarget:self.object];
    } else {
        [super forwardInvocation:anInvocation];
    }
}

- (NSUInteger)hash {
    return [self.object hash];
}

- (BOOL)isEqual:(id)otherObject {
    if (otherObject == nil) {
        return self.object == nil;
    }
    if (self == otherObject || self.object == otherObject) {
        return YES;
    }
    if ([otherObject isMemberOfClass:[WeakObject class]]) {
        WeakObject *other = (WeakObject *)otherObject;
        return [other isEqual:self.object];
    }
    return [self.object isEqual:otherObject];
}

@end
