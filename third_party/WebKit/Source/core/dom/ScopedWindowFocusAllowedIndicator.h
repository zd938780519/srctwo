// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScopedWindowFocusAllowedIndicator_h
#define ScopedWindowFocusAllowedIndicator_h

#include "core/dom/ExecutionContext.h"
#include "platform/wtf/Noncopyable.h"

namespace blink {

class ScopedWindowFocusAllowedIndicator final {
  USING_FAST_MALLOC(ScopedWindowFocusAllowedIndicator);
  WTF_MAKE_NONCOPYABLE(ScopedWindowFocusAllowedIndicator);

 public:
  explicit ScopedWindowFocusAllowedIndicator(
      ExecutionContext* execution_context)
      : execution_context_(execution_context) {
    execution_context->AllowWindowInteraction();
  }
  ~ScopedWindowFocusAllowedIndicator() {
    execution_context_->ConsumeWindowInteraction();
  }

 private:
  // This doesn't create a cycle because ScopedWindowFocusAllowedIndicator
  // is used only on a machine stack.
  Persistent<ExecutionContext> execution_context_;
};

}  // namespace blink

#endif  // ScopedWindowFocusAllowedIndicator_h
