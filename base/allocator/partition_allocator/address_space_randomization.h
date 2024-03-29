// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ALLOCATOR_PARTITION_ALLOCATOR_ADDRESS_SPACE_RANDOMIZATION
#define BASE_ALLOCATOR_PARTITION_ALLOCATOR_ADDRESS_SPACE_RANDOMIZATION

#include "base/base_export.h"

namespace base {

// Calculates a random preferred mapping address. In calculating an address, we
// balance good ASLR against not fragmenting the address space too badly.
BASE_EXPORT void* GetRandomPageBase();

}  // namespace base

#endif  // BASE_ALLOCATOR_PARTITION_ALLOCATOR_ADDRESS_SPACE_RANDOMIZATION
