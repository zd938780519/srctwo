// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/web_view/internal/cwv_html_element_internal.h"

#import <Foundation/Foundation.h>

#include "testing/gtest/include/gtest/gtest.h"
#import "testing/gtest_mac.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace ios_web_view {

// Tests CWVHTMLElementTest initialization.
TEST(CWVHTMLElementTest, Initialization) {
  NSURL* const hyperlink = [NSURL URLWithString:@"https://chromium.test"];
  NSURL* const mediaSource = [NSURL URLWithString:@"https://chromium/img.webp"];
  NSString* const text = @"link";
  CWVHTMLElement* element =
      [[CWVHTMLElement alloc] initWithHyperlink:hyperlink
                                    mediaSource:mediaSource
                                           text:text];
  EXPECT_NSEQ(hyperlink, element.hyperlink);
  EXPECT_NSEQ(mediaSource, element.mediaSource);
  EXPECT_NSEQ(text, element.text);
}

}  // namespace ios_web_view
