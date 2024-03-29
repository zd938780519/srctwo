// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/editing/commands/EditingState.h"

namespace blink {

EditingState::EditingState() {}

EditingState::~EditingState() {}

void EditingState::Abort() {
  DCHECK(!is_aborted_);
  is_aborted_ = true;
}

// ---
IgnorableEditingAbortState::IgnorableEditingAbortState() {}

IgnorableEditingAbortState::~IgnorableEditingAbortState() {}

#if DCHECK_IS_ON()
// ---

NoEditingAbortChecker::NoEditingAbortChecker(const char* file, int line)
    : file_(file), line_(line) {}

NoEditingAbortChecker::~NoEditingAbortChecker() {
  DCHECK_AT(!editing_state_.IsAborted(), file_, line_)
      << "The operation should not have been aborted.";
}

#endif

}  // namespace blink
