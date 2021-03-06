// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/video_capture/testing_controls_impl.h"

namespace video_capture {

TestingControlsImpl::TestingControlsImpl(
    std::unique_ptr<service_manager::ServiceContextRef> service_ref)
    : service_ref_(std::move(service_ref)) {}

TestingControlsImpl::~TestingControlsImpl() = default;

void TestingControlsImpl::Crash() {
  CHECK(false) << "This is an intentional crash for the purpose of testing";
}

}  // namespace video_capture
