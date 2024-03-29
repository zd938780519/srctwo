// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/search/one_google_bar/one_google_bar_data.h"

OneGoogleBarData::OneGoogleBarData() = default;
OneGoogleBarData::OneGoogleBarData(const OneGoogleBarData&) = default;
OneGoogleBarData::OneGoogleBarData(OneGoogleBarData&&) = default;
OneGoogleBarData::~OneGoogleBarData() = default;

OneGoogleBarData& OneGoogleBarData::operator=(const OneGoogleBarData&) =
    default;
OneGoogleBarData& OneGoogleBarData::operator=(OneGoogleBarData&&) = default;

bool operator==(const OneGoogleBarData& lhs, const OneGoogleBarData& rhs) {
  return lhs.bar_html == rhs.bar_html &&
         lhs.in_head_script == rhs.in_head_script &&
         lhs.in_head_style == rhs.in_head_style &&
         lhs.after_bar_script == rhs.after_bar_script &&
         lhs.end_of_body_html == rhs.end_of_body_html &&
         lhs.end_of_body_script == rhs.end_of_body_script;
}

bool operator!=(const OneGoogleBarData& lhs, const OneGoogleBarData& rhs) {
  return !(lhs == rhs);
}
