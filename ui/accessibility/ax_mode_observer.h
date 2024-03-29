// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_ACCESSIBILITY_PLATFORM_AX_MODE_OBSERVER_H_
#define UI_ACCESSIBILITY_PLATFORM_AX_MODE_OBSERVER_H_

#include "ui/accessibility/ax_export.h"
#include "ui/accessibility/ax_modes.h"

namespace ui {

class AX_EXPORT AXModeObserver {
 public:
  virtual ~AXModeObserver() {}

  // Notifies when accessibility mode changes.
  virtual void OnAXModeAdded(ui::AXMode mode) = 0;
};

}  // namespace ui

#endif  // UI_ACCESSIBILITY_PLATFORM_AX_MODE_OBSERVER_H_