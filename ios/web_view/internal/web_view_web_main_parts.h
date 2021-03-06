// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_WEB_VIEW_INTERNAL_WEB_VIEW_WEB_MAIN_PARTS_H_
#define IOS_WEB_VIEW_INTERNAL_WEB_VIEW_WEB_MAIN_PARTS_H_

#include <memory>

#include "base/macros.h"
#include "ios/web/public/app/web_main_parts.h"

namespace ios_web_view {

// WebView implementation of WebMainParts.
class WebViewWebMainParts : public web::WebMainParts {
 public:
  WebViewWebMainParts();
  ~WebViewWebMainParts() override;

 private:
  // web::WebMainParts implementation.
  void PreMainMessageLoopStart() override;
  void PreCreateThreads() override;
  void PreMainMessageLoopRun() override;
  void PostMainMessageLoopRun() override;
  void PostDestroyThreads() override;

  DISALLOW_COPY_AND_ASSIGN(WebViewWebMainParts);
};

}  // namespace ios_web_view

#endif  // IOS_WEB_VIEW_INTERNAL_WEB_VIEW_WEB_MAIN_PARTS_H_
