// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/geometry/DOMRectReadOnly.h"

#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8ObjectBuilder.h"
#include "core/geometry/DOMRectInit.h"

namespace blink {

DOMRectReadOnly* DOMRectReadOnly::Create(double x,
                                         double y,
                                         double width,
                                         double height) {
  return new DOMRectReadOnly(x, y, width, height);
}

ScriptValue DOMRectReadOnly::toJSONForBinding(ScriptState* script_state) const {
  V8ObjectBuilder result(script_state);
  result.AddNumber("x", x());
  result.AddNumber("y", y());
  result.AddNumber("width", width());
  result.AddNumber("height", height());
  result.AddNumber("top", top());
  result.AddNumber("right", right());
  result.AddNumber("bottom", bottom());
  result.AddNumber("left", left());
  return result.GetScriptValue();
}

DOMRectReadOnly* DOMRectReadOnly::FromIntRect(const IntRect& rect) {
  return new DOMRectReadOnly(rect.X(), rect.Y(), rect.Width(), rect.Height());
}

DOMRectReadOnly* DOMRectReadOnly::FromFloatRect(const FloatRect& rect) {
  return new DOMRectReadOnly(rect.X(), rect.Y(), rect.Width(), rect.Height());
}

DOMRectReadOnly* DOMRectReadOnly::fromRect(const DOMRectInit& other) {
  return new DOMRectReadOnly(other.x(), other.y(), other.width(),
                             other.height());
}

DOMRectReadOnly::DOMRectReadOnly(double x,
                                 double y,
                                 double width,
                                 double height)
    : x_(x), y_(y), width_(width), height_(height) {}

}  // namespace blink
