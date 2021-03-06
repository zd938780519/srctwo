// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/frame/immersive_mode_controller_stub.h"

#if defined(USE_ASH)
#include "chrome/browser/ui/views/frame/immersive_mode_controller_ash.h"
#endif

namespace chrome {

ImmersiveModeController* CreateImmersiveModeController() {
#if defined(USE_ASH)
  return new ImmersiveModeControllerAsh();
#else
  return new ImmersiveModeControllerStub();
#endif  // USE_ASH
}

}  // namespace chrome
