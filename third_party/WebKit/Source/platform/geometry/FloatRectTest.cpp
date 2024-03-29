// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/geometry/FloatRect.h"

#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/GeometryTestHelpers.h"
#include "platform/wtf/text/WTFString.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace blink {

TEST(FloatRectTest, SquaredDistanceToTest) {
  //
  //  O--x
  //  |
  //  y
  //
  //     FloatRect.x()   FloatRect.maxX()
  //            |          |
  //        1   |    2     |  3
  //      ======+==========+======   --FloatRect.y()
  //        4   |    5(in) |  6
  //      ======+==========+======   --FloatRect.maxY()
  //        7   |    8     |  9
  //

  FloatRect r1(100, 100, 250, 150);

  // `1` case
  FloatPoint p1(80, 80);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p1),
                      800.f);

  FloatPoint p2(-10, -10);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p2),
                      24200.f);

  FloatPoint p3(80, -10);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p3),
                      12500.f);

  // `2` case
  FloatPoint p4(110, 80);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p4),
                      400.f);

  FloatPoint p5(150, 0);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p5),
                      10000.f);

  FloatPoint p6(180, -10);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p6),
                      12100.f);

  // `3` case
  FloatPoint p7(400, 80);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p7),
                      2900.f);

  FloatPoint p8(360, -10);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p8),
                      12200.f);

  // `4` case
  FloatPoint p9(80, 110);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual, r1.SquaredDistanceTo(p9),
                      400.f);

  FloatPoint p10(-10, 180);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p10), 12100.f);

  // `5`(& In) case
  FloatPoint p11(100, 100);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p11), 0.f);

  FloatPoint p12(150, 100);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p12), 0.f);

  FloatPoint p13(350, 100);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p13), 0.f);

  FloatPoint p14(350, 150);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p14), 0.f);

  FloatPoint p15(350, 250);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p15), 0.f);

  FloatPoint p16(150, 250);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p16), 0.f);

  FloatPoint p17(100, 250);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p17), 0.f);

  FloatPoint p18(100, 150);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p18), 0.f);

  FloatPoint p19(150, 150);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p19), 0.f);

  // `6` case
  FloatPoint p20(380, 150);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p20), 900.f);

  // `7` case
  FloatPoint p21(80, 280);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p21), 1300.f);

  FloatPoint p22(-10, 300);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p22), 14600.f);

  // `8` case
  FloatPoint p23(180, 300);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p23), 2500.f);

  // `9` case
  FloatPoint p24(450, 450);
  EXPECT_PRED_FORMAT2(GeometryTest::AssertAlmostEqual,
                      r1.SquaredDistanceTo(p24), 50000.f);
}

TEST(FloatRectTest, ToString) {
  FloatRect empty_rect = FloatRect();
  EXPECT_EQ("0,0 0x0", empty_rect.ToString());

  FloatRect rect(1, 2, 3, 4);
  EXPECT_EQ("1,2 3x4", rect.ToString());

  FloatRect granular_rect(1.6f, 2.7f, 3.8f, 4.9f);
  EXPECT_EQ("1.6,2.7 3.8x4.9", granular_rect.ToString());

  FloatRect min_max_rect(
      std::numeric_limits<float>::min(), std::numeric_limits<float>::max(),
      std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
  EXPECT_EQ("1.17549e-38,3.40282e+38 1.17549e-38x3.40282e+38",
            min_max_rect.ToString());

  FloatRect infinite_rect(-std::numeric_limits<float>::infinity(),
                          -std::numeric_limits<float>::infinity(),
                          std::numeric_limits<float>::infinity(),
                          std::numeric_limits<float>::infinity());
  EXPECT_EQ("-inf,-inf infxinf", infinite_rect.ToString());

  FloatRect nan_rect(0, 0, std::numeric_limits<float>::signaling_NaN(),
                     std::numeric_limits<float>::signaling_NaN());
  EXPECT_EQ("0,0 nanxnan", nan_rect.ToString());
}

}  // namespace blink
