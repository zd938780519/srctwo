// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StackUtil_h
#define StackUtil_h

#include <stddef.h>
#include <stdint.h>
#include "build/build_config.h"
#include "platform/wtf/WTFExport.h"

namespace WTF {

WTF_EXPORT size_t GetUnderestimatedStackSize();
WTF_EXPORT void* GetStackStart();

namespace internal {

WTF_EXPORT extern uintptr_t g_main_thread_stack_start;
WTF_EXPORT extern uintptr_t g_main_thread_underestimated_stack_size;

WTF_EXPORT void InitializeMainThreadStackEstimate();

#if defined(OS_WIN) && defined(COMPILER_MSVC)
size_t ThreadStackSize();
#endif

}  // namespace internal

// Returns true if the function is not called on the main thread. Note carefully
// that this function may have false positives, i.e. it can return true even if
// we are on the main thread. If the function returns false, we are certainly
// on the main thread.
inline bool MayNotBeMainThread() {
  uintptr_t dummy;
  uintptr_t address_diff =
      internal::g_main_thread_stack_start - reinterpret_cast<uintptr_t>(&dummy);
  // This is a fast way to judge if we are in the main thread.
  // If |&dummy| is within |s_mainThreadUnderestimatedStackSize| byte from
  // the stack start of the main thread, we judge that we are in
  // the main thread.
  return address_diff >= internal::g_main_thread_underestimated_stack_size;
}

}  // namespace WTF

#endif  // StackUtil_h
