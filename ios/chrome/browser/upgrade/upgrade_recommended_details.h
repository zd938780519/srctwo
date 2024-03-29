// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UPGRADE_UPGRADE_RECOMMENDED_DETAILS_H_
#define IOS_CHROME_BROWSER_UPGRADE_UPGRADE_RECOMMENDED_DETAILS_H_

#include <string>

#include "url/gurl.h"

struct UpgradeRecommendedDetails {
  GURL upgrade_url;
  std::string next_version;
};

#endif  // IOS_CHROME_BROWSER_UPGRADE_UPGRADE_RECOMMENDED_DETAILS_H_
