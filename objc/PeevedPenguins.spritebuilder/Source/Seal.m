//
//  Seal.m
//  PeevedPenguins
//
//  Created by Jianjun on 13/5/14.
//  Copyright (c) 2014 Apportable. All rights reserved.
//

#import "Seal.h"

@implementation Seal

-(void)didLoadFromCCB
{
    self.physicsBody.collisionType = @"seal";
    CCLOG(@"load seal");
}


@end
