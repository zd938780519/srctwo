// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/session/session_observer.h"

#include "ash/session/session_controller.h"
#include "ash/shell.h"

namespace ash {

ScopedSessionObserver::ScopedSessionObserver(SessionObserver* observer)
    : observer_(observer) {
  // Shell may not exist in some tests.
  if (Shell::HasInstance())
    Shell::Get()->session_controller()->AddObserver(observer_);
}

ScopedSessionObserver::~ScopedSessionObserver() {
  if (Shell::HasInstance())
    Shell::Get()->session_controller()->RemoveObserver(observer_);
}

}  // namespace ash
