// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleOffsetRotation_h
#define StyleOffsetRotation_h

#include "core/style/ComputedStyleConstants.h"

namespace blink {

struct StyleOffsetRotation {
  StyleOffsetRotation(float angle, OffsetRotationType type)
      : angle(angle), type(type) {}

  bool operator==(const StyleOffsetRotation& other) const {
    return angle == other.angle && type == other.type;
  }
  bool operator!=(const StyleOffsetRotation& other) const {
    return !(*this == other);
  }

  float angle;
  OffsetRotationType type;
};

}  // namespace blink

#endif  // StyleOffsetRotation_h
