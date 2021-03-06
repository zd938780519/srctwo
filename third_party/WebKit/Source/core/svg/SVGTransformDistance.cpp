/*
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
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

#include "core/svg/SVGTransformDistance.h"

#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/FloatSize.h"
#include <math.h>

namespace blink {

SVGTransformDistance::SVGTransformDistance()
    : transform_type_(kSvgTransformUnknown), angle_(0), cx_(0), cy_(0) {}

SVGTransformDistance::SVGTransformDistance(SVGTransformType transform_type,
                                           float angle,
                                           float cx,
                                           float cy,
                                           const AffineTransform& transform)
    : transform_type_(transform_type),
      angle_(angle),
      cx_(cx),
      cy_(cy),
      transform_(transform) {}

SVGTransformDistance::SVGTransformDistance(SVGTransform* from_svg_transform,
                                           SVGTransform* to_svg_transform)
    : angle_(0), cx_(0), cy_(0) {
  transform_type_ = from_svg_transform->TransformType();
  DCHECK_EQ(transform_type_, to_svg_transform->TransformType());

  switch (transform_type_) {
    case kSvgTransformMatrix:
      NOTREACHED();
    case kSvgTransformUnknown:
      break;
    case kSvgTransformRotate: {
      FloatSize center_distance = to_svg_transform->RotationCenter() -
                                  from_svg_transform->RotationCenter();
      angle_ = to_svg_transform->Angle() - from_svg_transform->Angle();
      cx_ = center_distance.Width();
      cy_ = center_distance.Height();
      break;
    }
    case kSvgTransformTranslate: {
      FloatSize translation_distance =
          to_svg_transform->Translate() - from_svg_transform->Translate();
      transform_.Translate(translation_distance.Width(),
                           translation_distance.Height());
      break;
    }
    case kSvgTransformScale: {
      float scale_x = to_svg_transform->Scale().Width() -
                      from_svg_transform->Scale().Width();
      float scale_y = to_svg_transform->Scale().Height() -
                      from_svg_transform->Scale().Height();
      transform_.ScaleNonUniform(scale_x, scale_y);
      break;
    }
    case kSvgTransformSkewx:
    case kSvgTransformSkewy:
      angle_ = to_svg_transform->Angle() - from_svg_transform->Angle();
      break;
  }
}

SVGTransformDistance SVGTransformDistance::ScaledDistance(
    float scale_factor) const {
  switch (transform_type_) {
    case kSvgTransformMatrix:
      NOTREACHED();
    case kSvgTransformUnknown:
      return SVGTransformDistance();
    case kSvgTransformRotate:
      return SVGTransformDistance(transform_type_, angle_ * scale_factor,
                                  cx_ * scale_factor, cy_ * scale_factor,
                                  AffineTransform());
    case kSvgTransformScale:
      return SVGTransformDistance(
          transform_type_, angle_ * scale_factor, cx_ * scale_factor,
          cy_ * scale_factor, AffineTransform(transform_).Scale(scale_factor));
    case kSvgTransformTranslate: {
      AffineTransform new_transform(transform_);
      new_transform.SetE(transform_.E() * scale_factor);
      new_transform.SetF(transform_.F() * scale_factor);
      return SVGTransformDistance(transform_type_, 0, 0, 0, new_transform);
    }
    case kSvgTransformSkewx:
    case kSvgTransformSkewy:
      return SVGTransformDistance(transform_type_, angle_ * scale_factor,
                                  cx_ * scale_factor, cy_ * scale_factor,
                                  AffineTransform());
  }

  NOTREACHED();
  return SVGTransformDistance();
}

SVGTransform* SVGTransformDistance::AddSVGTransforms(SVGTransform* first,
                                                     SVGTransform* second,
                                                     unsigned repeat_count) {
  DCHECK_EQ(first->TransformType(), second->TransformType());

  SVGTransform* transform = SVGTransform::Create();

  switch (first->TransformType()) {
    case kSvgTransformMatrix:
      NOTREACHED();
    case kSvgTransformUnknown:
      return transform;
    case kSvgTransformRotate: {
      transform->SetRotate(first->Angle() + second->Angle() * repeat_count,
                           first->RotationCenter().X() +
                               second->RotationCenter().X() * repeat_count,
                           first->RotationCenter().Y() +
                               second->RotationCenter().Y() * repeat_count);
      return transform;
    }
    case kSvgTransformTranslate: {
      float dx =
          first->Translate().X() + second->Translate().X() * repeat_count;
      float dy =
          first->Translate().Y() + second->Translate().Y() * repeat_count;
      transform->SetTranslate(dx, dy);
      return transform;
    }
    case kSvgTransformScale: {
      FloatSize scale = second->Scale();
      scale.Scale(repeat_count);
      scale += first->Scale();
      transform->SetScale(scale.Width(), scale.Height());
      return transform;
    }
    case kSvgTransformSkewx:
      transform->SetSkewX(first->Angle() + second->Angle() * repeat_count);
      return transform;
    case kSvgTransformSkewy:
      transform->SetSkewY(first->Angle() + second->Angle() * repeat_count);
      return transform;
  }
  NOTREACHED();
  return transform;
}

SVGTransform* SVGTransformDistance::AddToSVGTransform(
    SVGTransform* transform) const {
  DCHECK(transform_type_ == transform->TransformType() ||
         transform_type_ == kSvgTransformUnknown);

  SVGTransform* new_transform = transform->Clone();

  switch (transform_type_) {
    case kSvgTransformMatrix:
      NOTREACHED();
    case kSvgTransformUnknown:
      return SVGTransform::Create();
    case kSvgTransformTranslate: {
      FloatPoint translation = transform->Translate();
      translation += FloatSize::NarrowPrecision(transform_.E(), transform_.F());
      new_transform->SetTranslate(translation.X(), translation.Y());
      return new_transform;
    }
    case kSvgTransformScale: {
      FloatSize scale = transform->Scale();
      scale += FloatSize::NarrowPrecision(transform_.A(), transform_.D());
      new_transform->SetScale(scale.Width(), scale.Height());
      return new_transform;
    }
    case kSvgTransformRotate: {
      FloatPoint center = transform->RotationCenter();
      new_transform->SetRotate(transform->Angle() + angle_, center.X() + cx_,
                               center.Y() + cy_);
      return new_transform;
    }
    case kSvgTransformSkewx:
      new_transform->SetSkewX(transform->Angle() + angle_);
      return new_transform;
    case kSvgTransformSkewy:
      new_transform->SetSkewY(transform->Angle() + angle_);
      return new_transform;
  }

  NOTREACHED();
  return new_transform;
}

float SVGTransformDistance::Distance() const {
  switch (transform_type_) {
    case kSvgTransformMatrix:
      NOTREACHED();
    case kSvgTransformUnknown:
      return 0;
    case kSvgTransformRotate:
      return sqrtf(angle_ * angle_ + cx_ * cx_ + cy_ * cy_);
    case kSvgTransformScale:
      return static_cast<float>(sqrt(transform_.A() * transform_.A() +
                                     transform_.D() * transform_.D()));
    case kSvgTransformTranslate:
      return static_cast<float>(sqrt(transform_.E() * transform_.E() +
                                     transform_.F() * transform_.F()));
    case kSvgTransformSkewx:
    case kSvgTransformSkewy:
      return angle_;
  }
  NOTREACHED();
  return 0;
}

}  // namespace blink
