// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_VIDEO_CAPTURE_TEST_FAKE_DEVICE_TEST_H_
#define SERVICES_VIDEO_CAPTURE_TEST_FAKE_DEVICE_TEST_H_

#include "media/capture/video_capture_types.h"
#include "services/video_capture/test/fake_device_descriptor_test.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace video_capture {

// Test fixture that creates a proxy to the fake device provided by the fake
// device factory.
class FakeDeviceTest : public FakeDeviceDescriptorTest {
 public:
  FakeDeviceTest();
  ~FakeDeviceTest() override;

  void SetUp() override;

 protected:
  media::VideoCaptureFormat fake_device_first_supported_format_;
  media::VideoCaptureParams requestable_settings_;
  mojom::DevicePtr fake_device_proxy_;
};

}  // namespace video_capture

#endif  // SERVICES_VIDEO_CAPTURE_TEST_FAKE_DEVICE_TEST_H_
