// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_TEST_EARL_GREY_ACCESSIBILITY_UTIL_H_
#define IOS_CHROME_TEST_EARL_GREY_ACCESSIBILITY_UTIL_H_

namespace chrome_test_util {

// Verifies that all interactive elements on screen (or at least one of their
// descendants) are accessible.
void VerifyAccessibilityForCurrentScreen();

}  // namespace chrome_test_util

#endif  // IOS_CHROME_TEST_EARL_GREY_ACCESSIBILITY_UTIL_H_
