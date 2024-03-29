// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/aura/client/drag_drop_delegate.h"

#include "ui/aura/window.h"
#include "ui/base/class_property.h"

DECLARE_UI_CLASS_PROPERTY_TYPE(aura::client::DragDropDelegate*)

namespace aura {
namespace client {

DEFINE_LOCAL_UI_CLASS_PROPERTY_KEY(DragDropDelegate*,
                                 kDragDropDelegateKey,
                                 nullptr);

void SetDragDropDelegate(Window* window, DragDropDelegate* delegate) {
  window->SetProperty(kDragDropDelegateKey, delegate);
}

DragDropDelegate* GetDragDropDelegate(Window* window) {
  return window->GetProperty(kDragDropDelegateKey);
}

}  // namespace client
}  // namespace aura
