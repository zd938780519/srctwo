// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/platform/WebMemoryCoordinator.h"

#include "platform/MemoryCoordinator.h"

namespace blink {

void WebMemoryCoordinator::OnMemoryPressure(
    WebMemoryPressureLevel pressure_level) {
  MemoryCoordinator::Instance().OnMemoryPressure(pressure_level);
}

void WebMemoryCoordinator::OnMemoryStateChange(MemoryState state) {
  MemoryCoordinator::Instance().OnMemoryStateChange(state);
}

void WebMemoryCoordinator::OnPurgeMemory() {
  MemoryCoordinator::Instance().OnPurgeMemory();
}

}  // namespace blink
