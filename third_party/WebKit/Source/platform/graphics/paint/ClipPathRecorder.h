// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ClipPathRecorder_h
#define ClipPathRecorder_h

#include "platform/graphics/Path.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "platform/wtf/Allocator.h"
#include "platform/wtf/Noncopyable.h"

namespace blink {

class GraphicsContext;

class PLATFORM_EXPORT ClipPathRecorder {
  USING_FAST_MALLOC(ClipPathRecorder);
  WTF_MAKE_NONCOPYABLE(ClipPathRecorder);

 public:
  ClipPathRecorder(GraphicsContext&, const DisplayItemClient&, const Path&);
  ~ClipPathRecorder();

 private:
  GraphicsContext& context_;
  const DisplayItemClient& client_;
};

}  // namespace blink

#endif  // ClipPathRecorder_h
