// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/animation/CompositorFloatKeyframe.h"

#include "platform/animation/TimingFunction.h"

namespace blink {

CompositorFloatKeyframe::CompositorFloatKeyframe(
    double time,
    float value,
    const TimingFunction& timing_function)
    : float_keyframe_(
          cc::FloatKeyframe::Create(base::TimeDelta::FromSecondsD(time),
                                    value,
                                    timing_function.CloneToCC())) {}

CompositorFloatKeyframe::CompositorFloatKeyframe(
    std::unique_ptr<cc::FloatKeyframe> float_keyframe)
    : float_keyframe_(std::move(float_keyframe)) {}

CompositorFloatKeyframe::~CompositorFloatKeyframe() {}

double CompositorFloatKeyframe::Time() const {
  return float_keyframe_->Time().InSecondsF();
}

const cc::TimingFunction* CompositorFloatKeyframe::CcTimingFunction() const {
  return float_keyframe_->timing_function();
}

std::unique_ptr<cc::FloatKeyframe> CompositorFloatKeyframe::CloneToCC() const {
  return float_keyframe_->Clone();
}

}  // namespace blink
