// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_WEB_VIEW_SHELL_TEST_EARL_GREY_WEB_VIEW_SHELL_MATCHERS_H_
#define IOS_WEB_VIEW_SHELL_TEST_EARL_GREY_WEB_VIEW_SHELL_MATCHERS_H_

#import <string>

#import <EarlGrey/EarlGrey.h>

namespace ios_web_view {

// Matcher for web view shell address field text property equal to |text|.
id<GREYMatcher> AddressFieldText(const std::string& text);

// Matcher for back button in web view shell.
id<GREYMatcher> BackButton();

// Matcher for forward button in web view shell.
id<GREYMatcher> ForwardButton();

// Matcher for address field in web view shell.
id<GREYMatcher> AddressField();

}  // namespace ios_web_view

#endif  // IOS_WEB_VIEW_SHELL_TEST_EARL_GREY_WEB_VIEW_SHELL_MATCHERS_H_
