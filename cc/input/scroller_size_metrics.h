// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_SCROLLER_SIZE_METRICS_H_
#define CC_INPUT_SCROLLER_SIZE_METRICS_H_

namespace cc {

// Use the two constants to add scroller size related UMA metrics.
// The goal of the metrics is to find the frequency of scrollers of
// different sizes that get scrolled. This experiment is aiming at small
// scrollers therefore the big ones, e.g. larger than 400px * 500px, will get
// capped into one bucket.
static constexpr int kScrollerSizeLargestBucket = 200000;
static constexpr int kScrollerSizeBucketCount = 50;

// Use the two constants to record GPU memory for tilings. Any layer that
// costs more than ~1GB will get capped.
static constexpr int kGPUMemoryForTilingsLargestBucketKb = 1000000;
static constexpr int kGPUMemoryForTilingsBucketCount = 50;
}  // namespace cc

#endif  // CC_INPUT_SCROLLER_SIZE_METRICS_H_
