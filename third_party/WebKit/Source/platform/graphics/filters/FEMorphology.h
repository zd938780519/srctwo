/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef FEMorphology_h
#define FEMorphology_h

#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

enum MorphologyOperatorType {
  FEMORPHOLOGY_OPERATOR_UNKNOWN = 0,
  FEMORPHOLOGY_OPERATOR_ERODE = 1,
  FEMORPHOLOGY_OPERATOR_DILATE = 2
};

class PLATFORM_EXPORT FEMorphology final : public FilterEffect {
 public:
  static FEMorphology* Create(Filter*,
                              MorphologyOperatorType,
                              float radius_x,
                              float radius_y);
  MorphologyOperatorType MorphologyOperator() const;
  bool SetMorphologyOperator(MorphologyOperatorType);

  float RadiusX() const;
  bool SetRadiusX(float);

  float RadiusY() const;
  bool SetRadiusY(float);

  TextStream& ExternalRepresentation(TextStream&, int indention) const override;

 private:
  FEMorphology(Filter*, MorphologyOperatorType, float radius_x, float radius_y);

  FloatRect MapEffect(const FloatRect&) const override;

  sk_sp<SkImageFilter> CreateImageFilter() override;

  MorphologyOperatorType type_;
  float radius_x_;
  float radius_y_;
};

}  // namespace blink

#endif  // FEMorphology_h
