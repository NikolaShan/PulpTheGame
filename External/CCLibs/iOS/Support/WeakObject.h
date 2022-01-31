//
//  WeakObject.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface WeakObject : NSObject

@property (nonatomic, readonly, weak) id object;

+ (instancetype)with:(id)object;
- (instancetype)initWithObject:(id)object;

@end
