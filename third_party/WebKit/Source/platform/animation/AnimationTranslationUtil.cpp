/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "platform/animation/AnimationTranslationUtil.h"

#include "platform/animation/CompositorTransformOperations.h"
#include "platform/transforms/InterpolatedTransformOperation.h"
#include "platform/transforms/Matrix3DTransformOperation.h"
#include "platform/transforms/MatrixTransformOperation.h"
#include "platform/transforms/PerspectiveTransformOperation.h"
#include "platform/transforms/RotateTransformOperation.h"
#include "platform/transforms/ScaleTransformOperation.h"
#include "platform/transforms/SkewTransformOperation.h"
#include "platform/transforms/TransformOperations.h"
#include "platform/transforms/TransformationMatrix.h"
#include "platform/transforms/TranslateTransformOperation.h"

namespace blink {

void ToCompositorTransformOperations(
    const TransformOperations& transform_operations,
    CompositorTransformOperations* out_transform_operations) {
  // We need to do a deep copy the transformOperations may contain ref pointers
  // to TransformOperation objects.
  for (const auto& operation : transform_operations.Operations()) {
    switch (operation->GetType()) {
      case TransformOperation::kScaleX:
      case TransformOperation::kScaleY:
      case TransformOperation::kScaleZ:
      case TransformOperation::kScale3D:
      case TransformOperation::kScale: {
        auto transform =
            static_cast<const ScaleTransformOperation*>(operation.Get());
        out_transform_operations->AppendScale(transform->X(), transform->Y(),
                                              transform->Z());
        break;
      }
      case TransformOperation::kTranslateX:
      case TransformOperation::kTranslateY:
      case TransformOperation::kTranslateZ:
      case TransformOperation::kTranslate3D:
      case TransformOperation::kTranslate: {
        auto transform =
            static_cast<const TranslateTransformOperation*>(operation.Get());
        DCHECK(transform->X().IsFixed() && transform->Y().IsFixed());
        out_transform_operations->AppendTranslate(
            transform->X().Value(), transform->Y().Value(), transform->Z());
        break;
      }
      case TransformOperation::kRotateX:
      case TransformOperation::kRotateY:
      case TransformOperation::kRotate3D:
      case TransformOperation::kRotate: {
        auto transform =
            static_cast<const RotateTransformOperation*>(operation.Get());
        out_transform_operations->AppendRotate(
            transform->X(), transform->Y(), transform->Z(), transform->Angle());
        break;
      }
      case TransformOperation::kSkewX:
      case TransformOperation::kSkewY:
      case TransformOperation::kSkew: {
        auto transform =
            static_cast<const SkewTransformOperation*>(operation.Get());
        out_transform_operations->AppendSkew(transform->AngleX(),
                                             transform->AngleY());
        break;
      }
      case TransformOperation::kMatrix: {
        auto transform =
            static_cast<const MatrixTransformOperation*>(operation.Get());
        TransformationMatrix m = transform->Matrix();
        out_transform_operations->AppendMatrix(
            TransformationMatrix::ToSkMatrix44(m));
        break;
      }
      case TransformOperation::kMatrix3D: {
        auto transform =
            static_cast<const Matrix3DTransformOperation*>(operation.Get());
        TransformationMatrix m = transform->Matrix();
        out_transform_operations->AppendMatrix(
            TransformationMatrix::ToSkMatrix44(m));
        break;
      }
      case TransformOperation::kPerspective: {
        auto transform =
            static_cast<const PerspectiveTransformOperation*>(operation.Get());
        out_transform_operations->AppendPerspective(transform->Perspective());
        break;
      }
      case TransformOperation::kRotateAroundOrigin:
      case TransformOperation::kInterpolated: {
        TransformationMatrix m;
        operation->Apply(m, FloatSize());
        out_transform_operations->AppendMatrix(
            TransformationMatrix::ToSkMatrix44(m));
        break;
      }
      case TransformOperation::kIdentity:
        out_transform_operations->AppendIdentity();
        break;
      default:
        NOTREACHED();
        break;
    }  // switch
  }    // for each operation
}

}  // namespace blink
