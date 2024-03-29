// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGShapePainter_h
#define SVGShapePainter_h

#include "platform/graphics/paint/PaintFlags.h"
#include "platform/wtf/Allocator.h"
#include "third_party/skia/include/core/SkPath.h"

namespace blink {

class FloatRect;
class GraphicsContext;
class LayoutSVGResourceMarker;
class LayoutSVGShape;
struct MarkerPosition;
struct PaintInfo;

class SVGShapePainter {
  STACK_ALLOCATED();

 public:
  SVGShapePainter(const LayoutSVGShape& layout_svg_shape)
      : layout_svg_shape_(layout_svg_shape) {}

  void Paint(const PaintInfo&);

 private:
  void FillShape(GraphicsContext&, const PaintFlags&, SkPath::FillType);
  void StrokeShape(GraphicsContext&, const PaintFlags&);

  void PaintMarkers(const PaintInfo&, const FloatRect& bounding_box);
  void PaintMarker(const PaintInfo&,
                   const LayoutSVGResourceMarker&,
                   const MarkerPosition&,
                   float stroke_width);

  const LayoutSVGShape& layout_svg_shape_;
};

}  // namespace blink

#endif  // SVGShapePainter_h
