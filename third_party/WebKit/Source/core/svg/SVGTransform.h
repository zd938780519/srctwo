/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGTransform_h
#define SVGTransform_h

#include "core/svg/properties/SVGProperty.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/transforms/AffineTransform.h"
#include "platform/wtf/text/WTFString.h"

namespace blink {

class FloatSize;
class SVGTransformTearOff;

enum SVGTransformType {
  kSvgTransformUnknown = 0,
  kSvgTransformMatrix = 1,
  kSvgTransformTranslate = 2,
  kSvgTransformScale = 3,
  kSvgTransformRotate = 4,
  kSvgTransformSkewx = 5,
  kSvgTransformSkewy = 6
};

class SVGTransform final : public SVGPropertyBase {
 public:
  typedef SVGTransformTearOff TearOffType;

  enum ConstructionMode {
    kConstructIdentityTransform,
    kConstructZeroTransform
  };

  static SVGTransform* Create() { return new SVGTransform(); }

  static SVGTransform* Create(
      SVGTransformType type,
      ConstructionMode mode = kConstructIdentityTransform) {
    return new SVGTransform(type, mode);
  }

  static SVGTransform* Create(const AffineTransform& affine_transform) {
    return new SVGTransform(affine_transform);
  }

  ~SVGTransform() override;

  SVGTransform* Clone() const;
  SVGPropertyBase* CloneForAnimation(const String&) const override;

  SVGTransformType TransformType() const { return transform_type_; }

  const AffineTransform& Matrix() const { return matrix_; }

  // |onMatrixChange| must be called after modifications via |mutableMatrix|.
  AffineTransform* MutableMatrix() { return &matrix_; }
  void OnMatrixChange();

  float Angle() const { return angle_; }
  FloatPoint RotationCenter() const { return center_; }

  void SetMatrix(const AffineTransform&);
  void SetTranslate(float tx, float ty);
  void SetScale(float sx, float sy);
  void SetRotate(float angle, float cx, float cy);
  void SetSkewX(float angle);
  void SetSkewY(float angle);

  // Internal use only (animation system)
  FloatPoint Translate() const;
  FloatSize Scale() const;

  String ValueAsString() const override;

  void Add(SVGPropertyBase*, SVGElement*) override;
  void CalculateAnimatedValue(SVGAnimationElement*,
                              float percentage,
                              unsigned repeat_count,
                              SVGPropertyBase* from,
                              SVGPropertyBase* to,
                              SVGPropertyBase* to_at_end_of_duration_value,
                              SVGElement* context_element) override;
  float CalculateDistance(SVGPropertyBase* to,
                          SVGElement* context_element) override;

  static AnimatedPropertyType ClassType() { return kAnimatedTransform; }
  AnimatedPropertyType GetType() const override { return ClassType(); }

 private:
  SVGTransform();
  SVGTransform(SVGTransformType, ConstructionMode);
  explicit SVGTransform(const AffineTransform&);
  SVGTransform(SVGTransformType,
               float,
               const FloatPoint&,
               const AffineTransform&);

  SVGTransformType transform_type_;
  float angle_;
  FloatPoint center_;
  AffineTransform matrix_;
};

}  // namespace blink

#endif  // SVGTransform_h
