// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/ui/context_menu/context_menu_coordinator.h"

#import <UIKit/UIKit.h>

#import "base/mac/scoped_nsobject.h"
#import "ios/chrome/browser/ui/alert_coordinator/action_sheet_coordinator.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/strings/grit/ui_strings.h"
#import "ios/web/public/web_state/context_menu_params.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

@interface ContextMenuCoordinator () {
  // Coordinator handling the alert.
  base::scoped_nsobject<ActionSheetCoordinator> _alertCoordinator;
}
@end

@implementation ContextMenuCoordinator

- (instancetype)initWithBaseViewController:(UIViewController*)viewController
                                    params:
                                        (const web::ContextMenuParams&)params {
  self = [super initWithBaseViewController:viewController];
  if (self) {
    CGRect rect = CGRectMake(params.location.x, params.location.y, 1.0, 1.0);
    _alertCoordinator.reset([[ActionSheetCoordinator alloc]
        initWithBaseViewController:viewController
                             title:params.menu_title
                           message:nil
                              rect:rect
                              view:params.view]);
  }
  return self;
}

- (instancetype)initWithBaseViewController:(UIViewController*)viewController {
  NOTREACHED();
  return nil;
}

#pragma mark - Public Methods

- (void)addItemWithTitle:(NSString*)title action:(ProceduralBlock)actionBlock {
  [_alertCoordinator addItemWithTitle:title
                               action:actionBlock
                                style:UIAlertActionStyleDefault];
}

- (void)start {
  [_alertCoordinator addItemWithTitle:l10n_util::GetNSString(IDS_APP_CANCEL)
                               action:nil
                                style:UIAlertActionStyleCancel];
  [_alertCoordinator start];
}

- (void)stop {
  [_alertCoordinator stop];
}

#pragma mark - Property Implementation

- (BOOL)isVisible {
  return [_alertCoordinator isVisible];
}

@end
