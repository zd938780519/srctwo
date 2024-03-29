// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/shadow_value.h"

#include <stddef.h>

#include <algorithm>

#include "base/strings/stringprintf.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

namespace gfx {

namespace {

Insets GetInsets(const ShadowValues& shadows, bool include_inner_blur) {
  int left = 0;
  int top = 0;
  int right = 0;
  int bottom = 0;

  for (size_t i = 0; i < shadows.size(); ++i) {
    const ShadowValue& shadow = shadows[i];

    double blur = shadow.blur();
    if (!include_inner_blur)
      blur /= 2;
    // Add 0.5 to round up to the next integer.
    int blur_length = static_cast<int>(blur + 0.5);

    left = std::max(left, blur_length - shadow.x());
    top = std::max(top, blur_length - shadow.y());
    right = std::max(right, blur_length + shadow.x());
    bottom = std::max(bottom, blur_length + shadow.y());
  }

  return Insets(top, left, bottom, right);
}

}  // namespace

ShadowValue::ShadowValue() : blur_(0), color_(0) {}

ShadowValue::ShadowValue(const gfx::Vector2d& offset,
                         double blur,
                         SkColor color)
    : offset_(offset), blur_(blur), color_(color) {
}

ShadowValue::~ShadowValue() {}

ShadowValue ShadowValue::Scale(float scale) const {
  gfx::Vector2d scaled_offset =
      gfx::ToFlooredVector2d(gfx::ScaleVector2d(offset_, scale));
  return ShadowValue(scaled_offset, blur_ * scale, color_);
}

std::string ShadowValue::ToString() const {
  return base::StringPrintf(
      "(%d,%d),%.2f,rgba(%d,%d,%d,%d)",
      offset_.x(), offset_.y(),
      blur_,
      SkColorGetR(color_),
      SkColorGetG(color_),
      SkColorGetB(color_),
      SkColorGetA(color_));
}

// static
Insets ShadowValue::GetMargin(const ShadowValues& shadows) {
  gfx::Insets margins = GetInsets(shadows, false);
  return -margins;
}

// static
Insets ShadowValue::GetBlurRegion(const ShadowValues& shadows) {
  return GetInsets(shadows, true);
}

// static
ShadowValues ShadowValue::MakeMdShadowValues(int elevation) {
  ShadowValues shadow_values;
  // To match the CSS notion of blur (spread outside the bounding box) to the
  // Skia notion of blur (spread outside and inside the bounding box), we have
  // to double the designer-provided blur values.
  const int kBlurCorrection = 2;
  // "Key shadow": y offset is elevation and blur is twice the elevation.
  shadow_values.emplace_back(gfx::Vector2d(0, elevation),
                             kBlurCorrection * elevation * 2,
                             SkColorSetA(SK_ColorBLACK, 0x3d));
  // "Ambient shadow": no offset and blur matches the elevation.
  shadow_values.emplace_back(gfx::Vector2d(), kBlurCorrection * elevation,
                             SkColorSetA(SK_ColorBLACK, 0x1f));
  // To see what this looks like for elevation 24, try this CSS:
  //   box-shadow: 0 24px 48px rgba(0, 0, 0, .24),
  //               0 0 24px rgba(0, 0, 0, .12);
  return shadow_values;
}

}  // namespace gfx
