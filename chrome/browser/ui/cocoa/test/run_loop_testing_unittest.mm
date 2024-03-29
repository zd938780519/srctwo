// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/cocoa/test/run_loop_testing.h"

#import <Foundation/Foundation.h>

#include "base/mac/scoped_nsobject.h"
#include "testing/gtest/include/gtest/gtest.h"

@interface TestDelayed : NSObject {
 @private
  BOOL didWork_;
  TestDelayed* next_;
}
@property(readonly, nonatomic) BOOL didWork;
@property(assign, nonatomic) TestDelayed* next;
@end

@implementation TestDelayed
@synthesize didWork = didWork_;
@synthesize next = next_;

- (id)init {
  if ((self = [super init])) {
    [self performSelector:@selector(doWork) withObject:nil afterDelay:0];
  }
  return self;
}

- (void)doWork {
  didWork_ = YES;
  [next_ performSelector:@selector(doWork) withObject:nil afterDelay:0];
}
@end

TEST(RunLoopTesting, RunAllPending) {
  base::scoped_nsobject<TestDelayed> tester([[TestDelayed alloc] init]);
  EXPECT_FALSE([tester didWork]);

  chrome::testing::NSRunLoopRunAllPending();

  EXPECT_TRUE([tester didWork]);
}

TEST(RunLoopTesting, NestedWork) {
  base::scoped_nsobject<TestDelayed> tester([[TestDelayed alloc] init]);
  base::scoped_nsobject<TestDelayed> nested([[TestDelayed alloc] init]);
  [tester setNext:nested];

  EXPECT_FALSE([tester didWork]);
  EXPECT_FALSE([nested didWork]);

  chrome::testing::NSRunLoopRunAllPending();

  EXPECT_TRUE([tester didWork]);
  EXPECT_TRUE([nested didWork]);
}
