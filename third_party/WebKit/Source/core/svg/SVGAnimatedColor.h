/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SVGAnimatedColor_h
#define SVGAnimatedColor_h

#include "core/css/StyleColor.h"
#include "core/svg/properties/SVGProperty.h"

namespace blink {

class SVGAnimationElement;

// StyleColor adaptor to SVGPropertyBase. This is only used for SMIL animations.
// FIXME: WebAnimations: Replacable with AnimatableColor once SMIL animations
// are implemented in WebAnimations.
class SVGColorProperty final : public SVGPropertyBase {
 public:
  static SVGColorProperty* Create(const String& color_string) {
    return new SVGColorProperty(color_string);
  }

  SVGPropertyBase* CloneForAnimation(const String&) const override;
  String ValueAsString() const override;

  void Add(SVGPropertyBase*, SVGElement*) override;
  void CalculateAnimatedValue(SVGAnimationElement*,
                              float percentage,
                              unsigned repeat_count,
                              SVGPropertyBase* from,
                              SVGPropertyBase* to,
                              SVGPropertyBase* to_at_end_of_duration_value,
                              SVGElement*) override;
  float CalculateDistance(SVGPropertyBase* to, SVGElement*) override;

  static AnimatedPropertyType ClassType() { return kAnimatedColor; }
  AnimatedPropertyType GetType() const override { return ClassType(); }

 private:
  explicit SVGColorProperty(const String&);

  StyleColor style_color_;
};

DEFINE_SVG_PROPERTY_TYPE_CASTS(SVGColorProperty);

}  // namespace blink

#endif  // SVGAnimatedColor_h
