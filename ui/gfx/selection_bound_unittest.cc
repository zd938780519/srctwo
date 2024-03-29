// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/selection_bound.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {

TEST(SelectionBoundTest, RectBetweenSelectionBounds) {
  SelectionBound b1, b2;
  // Simple case of aligned vertical bounds of equal height
  b1.SetEdge(gfx::PointF(0.f, 20.f), gfx::PointF(0.f, 25.f));
  b2.SetEdge(gfx::PointF(110.f, 20.f), gfx::PointF(110.f, 25.f));
  gfx::Rect expected_rect(
      b1.edge_top_rounded().x(), b1.edge_top_rounded().y(),
      b2.edge_top_rounded().x() - b1.edge_top_rounded().x(),
      b2.edge_bottom_rounded().y() - b2.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));

  // Parallel vertical bounds of different heights
  b1.SetEdge(gfx::PointF(10.f, 20.f), gfx::PointF(10.f, 25.f));
  b2.SetEdge(gfx::PointF(110.f, 0.f), gfx::PointF(110.f, 35.f));
  expected_rect =
      gfx::Rect(b1.edge_top_rounded().x(), b2.edge_top_rounded().y(),
                b2.edge_top_rounded().x() - b1.edge_top_rounded().x(),
                b2.edge_bottom_rounded().y() - b2.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));

  b1.SetEdge(gfx::PointF(10.f, 20.f), gfx::PointF(10.f, 30.f));
  b2.SetEdge(gfx::PointF(110.f, 25.f), gfx::PointF(110.f, 45.f));
  expected_rect =
      gfx::Rect(b1.edge_top_rounded().x(), b1.edge_top_rounded().y(),
                b2.edge_top_rounded().x() - b1.edge_top_rounded().x(),
                b2.edge_bottom_rounded().y() - b1.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));

  b1.SetEdge(gfx::PointF(10.f, 20.f), gfx::PointF(10.f, 30.f));
  b2.SetEdge(gfx::PointF(110.f, 40.f), gfx::PointF(110.f, 60.f));
  expected_rect =
      gfx::Rect(b1.edge_top_rounded().x(), b1.edge_top_rounded().y(),
                b2.edge_top_rounded().x() - b1.edge_top_rounded().x(),
                b2.edge_bottom_rounded().y() - b1.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));

  // Overlapping vertical bounds
  b1.SetEdge(gfx::PointF(10.f, 20.f), gfx::PointF(10.f, 30.f));
  b2.SetEdge(gfx::PointF(10.f, 25.f), gfx::PointF(10.f, 40.f));
  expected_rect =
      gfx::Rect(b1.edge_top_rounded().x(), b1.edge_top_rounded().y(), 0,
                b2.edge_bottom_rounded().y() - b1.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));

  // Non-vertical bounds: "\ \"
  b1.SetEdge(gfx::PointF(10.f, 20.f), gfx::PointF(20.f, 30.f));
  b2.SetEdge(gfx::PointF(110.f, 40.f), gfx::PointF(120.f, 60.f));
  expected_rect =
      gfx::Rect(b1.edge_top_rounded().x(), b1.edge_top_rounded().y(),
                b2.edge_bottom_rounded().x() - b1.edge_top_rounded().x(),
                b2.edge_bottom_rounded().y() - b1.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));

  // Non-vertical bounds: "/ \"
  b1.SetEdge(gfx::PointF(20.f, 30.f), gfx::PointF(0.f, 40.f));
  b2.SetEdge(gfx::PointF(110.f, 30.f), gfx::PointF(120.f, 40.f));
  expected_rect =
      gfx::Rect(b1.edge_bottom_rounded().x(), b1.edge_top_rounded().y(),
                b2.edge_bottom_rounded().x() - b1.edge_bottom_rounded().x(),
                b2.edge_bottom_rounded().y() - b2.edge_top_rounded().y());
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b1, b2));
  EXPECT_EQ(expected_rect, RectBetweenSelectionBounds(b2, b1));
}

}  // namespace gfx
