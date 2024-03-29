// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_PASSWORDS_UPDATE_PASSWORD_INFOBAR_CONTROLLER_H_
#define IOS_CHROME_BROWSER_PASSWORDS_UPDATE_PASSWORD_INFOBAR_CONTROLLER_H_

#include "ios/chrome/browser/infobars/confirm_infobar_controller.h"

class IOSChromeUpdatePasswordInfoBarDelegate;

// Controller for the Update Password info bar. Presents an info bar that asks
// the user whether they want to update their password.
@interface UpdatePasswordInfoBarController : ConfirmInfoBarController

- (InfoBarView*)viewForDelegate:
                    (IOSChromeUpdatePasswordInfoBarDelegate*)delegate
                          frame:(CGRect)frame;

@end

#endif  // IOS_CHROME_BROWSER_PASSWORDS_UPDATE_PASSWORD_INFOBAR_CONTROLLER_H_
