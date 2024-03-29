// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_ANIMATION_INK_DROP_OBSERVER_H_
#define UI_VIEWS_ANIMATION_INK_DROP_OBSERVER_H_

#include <string>

#include "base/macros.h"
#include "ui/views/views_export.h"

namespace views {

// Observer to attach to an InkDrop.
class VIEWS_EXPORT InkDropObserver {
 public:
  // Called when the animation of the current InkDrop has started. This
  // includes the ripple or highlight animation. Note: this is not guaranteed to
  // be notified, as the notification is dependent on the subclass
  // implementation.
  virtual void InkDropAnimationStarted() = 0;

 protected:
  InkDropObserver() = default;
  virtual ~InkDropObserver() = default;

 private:
  DISALLOW_COPY_AND_ASSIGN(InkDropObserver);
};

}  // namespace views

#endif  // UI_VIEWS_ANIMATION_INK_DROP_OBSERVER_H_
