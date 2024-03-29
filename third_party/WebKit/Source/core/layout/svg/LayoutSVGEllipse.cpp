/*
 * Copyright (C) 2012 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/layout/svg/LayoutSVGEllipse.h"

#include "core/svg/SVGCircleElement.h"
#include "core/svg/SVGEllipseElement.h"
#include <cmath>

namespace blink {

LayoutSVGEllipse::LayoutSVGEllipse(SVGGeometryElement* node)
    : LayoutSVGShape(node), use_path_fallback_(false) {}

LayoutSVGEllipse::~LayoutSVGEllipse() {}

void LayoutSVGEllipse::UpdateShapeFromElement() {
  // Before creating a new object we need to clear the cached bounding box
  // to avoid using garbage.
  fill_bounding_box_ = FloatRect();
  stroke_bounding_box_ = FloatRect();
  center_ = FloatPoint();
  radii_ = FloatSize();
  use_path_fallback_ = false;

  CalculateRadiiAndCenter();

  // Spec: "A negative value is an error. A value of zero disables rendering of
  // the element."
  if (radii_.Width() < 0 || radii_.Height() < 0)
    return;

  if (!radii_.IsEmpty()) {
    // Fall back to LayoutSVGShape and path-based hit detection if the ellipse
    // has a non-scaling or discontinuous stroke.
    if (HasNonScalingStroke() || !HasContinuousStroke()) {
      LayoutSVGShape::UpdateShapeFromElement();
      use_path_fallback_ = true;
      return;
    }
  }

  ClearPath();

  fill_bounding_box_ =
      FloatRect(center_.X() - radii_.Width(), center_.Y() - radii_.Height(),
                2 * radii_.Width(), 2 * radii_.Height());
  stroke_bounding_box_ = fill_bounding_box_;
  if (Style()->SvgStyle().HasStroke())
    stroke_bounding_box_.Inflate(StrokeWidth() / 2);
  if (GetElement())
    GetElement()->SetNeedsResizeObserverUpdate();
}

void LayoutSVGEllipse::CalculateRadiiAndCenter() {
  DCHECK(GetElement());
  SVGLengthContext length_context(GetElement());
  center_ = FloatPoint(
      length_context.ValueForLength(Style()->SvgStyle().Cx(), StyleRef(),
                                    SVGLengthMode::kWidth),
      length_context.ValueForLength(Style()->SvgStyle().Cy(), StyleRef(),
                                    SVGLengthMode::kHeight));

  if (isSVGCircleElement(*GetElement())) {
    float radius = length_context.ValueForLength(
        Style()->SvgStyle().R(), StyleRef(), SVGLengthMode::kOther);
    radii_ = FloatSize(radius, radius);
  } else {
    radii_ = FloatSize(
        length_context.ValueForLength(Style()->SvgStyle().Rx(), StyleRef(),
                                      SVGLengthMode::kWidth),
        length_context.ValueForLength(Style()->SvgStyle().Ry(), StyleRef(),
                                      SVGLengthMode::kHeight));
  }
}

bool LayoutSVGEllipse::ShapeDependentStrokeContains(const FloatPoint& point) {
  // The optimized check below for circles does not support non-scaling or
  // discontinuous strokes.
  if (use_path_fallback_ || !HasContinuousStroke() ||
      radii_.Width() != radii_.Height()) {
    if (!HasPath())
      CreatePath();
    return LayoutSVGShape::ShapeDependentStrokeContains(point);
  }

  const FloatPoint center =
      FloatPoint(center_.X() - point.X(), center_.Y() - point.Y());
  const float half_stroke_width = StrokeWidth() / 2;
  const float r = radii_.Width();
  return std::abs(center.length() - r) <= half_stroke_width;
}

bool LayoutSVGEllipse::ShapeDependentFillContains(
    const FloatPoint& point,
    const WindRule fill_rule) const {
  const FloatPoint center =
      FloatPoint(center_.X() - point.X(), center_.Y() - point.Y());

  // This works by checking if the point satisfies the ellipse equation.
  // (x/rX)^2 + (y/rY)^2 <= 1
  const float xr_x = center.X() / radii_.Width();
  const float yr_y = center.Y() / radii_.Height();
  return xr_x * xr_x + yr_y * yr_y <= 1.0;
}

bool LayoutSVGEllipse::HasContinuousStroke() const {
  const SVGComputedStyle& svg_style = Style()->SvgStyle();
  return svg_style.StrokeDashArray()->IsEmpty();
}

}  // namespace blink
