// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/wm/lock_state_controller_test_api.h"

namespace ash {

LockStateControllerTestApi::LockStateControllerTestApi(
    LockStateController* controller)
    : controller_(controller) {
  DCHECK(controller);
}

LockStateControllerTestApi::~LockStateControllerTestApi() {}

}  // namespace ash
