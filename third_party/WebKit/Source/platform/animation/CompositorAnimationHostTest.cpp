// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/animation/CompositorAnimationHost.h"

#include "base/memory/ref_counted.h"
#include "platform/animation/CompositorAnimationTimeline.h"
#include "platform/testing/CompositorTest.h"
#include "platform/testing/WebLayerTreeViewImplForTesting.h"
#include <memory>

namespace blink {

class CompositorAnimationHostTest : public CompositorTest {};

TEST_F(CompositorAnimationHostTest, AnimationHostNullWhenTimelineDetached) {
  std::unique_ptr<CompositorAnimationTimeline> timeline =
      CompositorAnimationTimeline::Create();

  scoped_refptr<cc::AnimationTimeline> cc_timeline =
      timeline->GetAnimationTimeline();
  EXPECT_FALSE(cc_timeline->animation_host());

  WebLayerTreeViewImplForTesting layer_tree_view;
  CompositorAnimationHost compositor_animation_host(
      layer_tree_view.CompositorAnimationHost());

  compositor_animation_host.AddTimeline(*timeline);
  EXPECT_TRUE(cc_timeline->animation_host());

  compositor_animation_host.RemoveTimeline(*timeline);
  EXPECT_FALSE(cc_timeline->animation_host());
}

}  // namespace blink
