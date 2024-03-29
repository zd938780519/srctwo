/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. AND ITS CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GOOGLE INC.
 * OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PageOverlay_h
#define PageOverlay_h

#include <memory>
#include "core/CoreExport.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/GraphicsLayerClient.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "platform/wtf/text/WTFString.h"

namespace blink {

class GraphicsContext;
class WebLocalFrameImpl;

// Manages a layer that is overlaid on a WebLocalFrame's content.
class CORE_EXPORT PageOverlay : public GraphicsLayerClient,
                                public DisplayItemClient {
 public:
  class Delegate {
   public:
    virtual ~Delegate() {}

    // Paints page overlay contents.
    virtual void PaintPageOverlay(const PageOverlay&,
                                  GraphicsContext&,
                                  const WebSize& web_view_size) const = 0;
  };

  static std::unique_ptr<PageOverlay> Create(
      WebLocalFrameImpl*,
      std::unique_ptr<PageOverlay::Delegate>);

  ~PageOverlay();

  void Update();

  GraphicsLayer* GetGraphicsLayer() const { return layer_.get(); }

  // DisplayItemClient methods.
  String DebugName() const final { return "PageOverlay"; }
  LayoutRect VisualRect() const override;

  // GraphicsLayerClient implementation
  bool NeedsRepaint(const GraphicsLayer&) const { return true; }
  IntRect ComputeInterestRect(const GraphicsLayer*,
                              const IntRect&) const override;
  void PaintContents(const GraphicsLayer*,
                     GraphicsContext&,
                     GraphicsLayerPaintingPhase,
                     const IntRect& interest_rect) const override;
  String DebugName(const GraphicsLayer*) const override;

 private:
  PageOverlay(WebLocalFrameImpl*, std::unique_ptr<PageOverlay::Delegate>);

  Persistent<WebLocalFrameImpl> frame_impl_;
  std::unique_ptr<PageOverlay::Delegate> delegate_;
  std::unique_ptr<GraphicsLayer> layer_;
};

}  // namespace blink

#endif  // PageOverlay_h
