// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/wm/public/activation_change_observer.h"

#include "ui/aura/window.h"
#include "ui/base/class_property.h"

DECLARE_UI_CLASS_PROPERTY_TYPE(wm::ActivationChangeObserver*)

namespace wm {

DEFINE_LOCAL_UI_CLASS_PROPERTY_KEY(
    ActivationChangeObserver*, kActivationChangeObserverKey, NULL);

void SetActivationChangeObserver(aura::Window* window,
                                 ActivationChangeObserver* observer) {
  window->SetProperty(kActivationChangeObserverKey, observer);
}

ActivationChangeObserver* GetActivationChangeObserver(aura::Window* window) {
  return window ? window->GetProperty(kActivationChangeObserverKey) : NULL;
}

}  // namespace wm
