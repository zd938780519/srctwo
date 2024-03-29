// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/web/public/url_util.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace web {

TEST(URLUtilTest, GURLByRemovingRefFromGURL) {
  GURL url("http://foo.com/bar#baz");
  EXPECT_EQ(GURL("http://foo.com/bar"), GURLByRemovingRefFromGURL(url));
}

}  // namespace web
