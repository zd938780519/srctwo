// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_ROUTER_MOCK_SCREEN_AVAILABILITY_LISTENER_H_
#define CHROME_BROWSER_MEDIA_ROUTER_MOCK_SCREEN_AVAILABILITY_LISTENER_H_

#include "content/public/browser/presentation_screen_availability_listener.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace media_router {

class MockScreenAvailabilityListener
    : public content::PresentationScreenAvailabilityListener {
 public:
  explicit MockScreenAvailabilityListener(const GURL& availability_url);
  ~MockScreenAvailabilityListener() override;

  GURL GetAvailabilityUrl() const override;

  MOCK_METHOD1(OnScreenAvailabilityChanged,
               void(blink::mojom::ScreenAvailability));

 private:
  GURL availability_url_;
};

}  // namespace media_router

#endif  // CHROME_BROWSER_MEDIA_ROUTER_MOCK_SCREEN_AVAILABILITY_LISTENER_H_
