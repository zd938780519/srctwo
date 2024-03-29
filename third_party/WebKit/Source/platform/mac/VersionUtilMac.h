// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VersionUtilMac_h
#define VersionUtilMac_h

#include "platform/PlatformExport.h"

#include <AvailabilityMacros.h>

namespace blink {

namespace internal {

PLATFORM_EXPORT int MacOSXMinorVersion();

template <int V, int ID>
constexpr bool IsOS() {
  return MAC_OS_X_VERSION_MIN_REQUIRED <= ID && MacOSXMinorVersion() == V;
}

}  // namespace internal

const auto IsOS10_9 = internal::IsOS<9, 1090>;
const auto IsOS10_10 = internal::IsOS<10, 101000>;
const auto IsOS10_11 = internal::IsOS<11, 101100>;
const auto IsOS10_12 = internal::IsOS<12, 101200>;

}  // namespace blink

#endif  // VersionUtilMac_h
