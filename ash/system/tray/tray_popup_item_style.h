// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_SYSTEM_TRAY_TRAY_POPUP_ITEM_STYLE_H_
#define ASH_SYSTEM_TRAY_TRAY_POPUP_ITEM_STYLE_H_

#include "base/macros.h"
#include "third_party/skia/include/core/SkColor.h"

namespace views {
class Label;
}  // namespace views

namespace ash {

// Central style provider for the system tray menu. Makes it easier to ensure
// all visuals are consistent and easily updated in one spot instead of being
// defined in multiple places throughout the code.
class TrayPopupItemStyle {
 public:
  // The different visual styles that a row can have.
  enum class ColorStyle {
    // Active and clickable.
    ACTIVE,
    // Inactive but clickable.
    INACTIVE,
    // Disabled and not clickable.
    DISABLED,
    // Color for "Connected" labels.
    CONNECTED,
  };

  // The different font styles that row text can have.
  enum class FontStyle {
    // Topmost header rows for default view and detailed view.
    TITLE,
    // Main text used by default view rows.
    DEFAULT_VIEW_LABEL,
    // Text in sub-section header rows in detailed views.
    SUB_HEADER,
    // Main text used by detailed view rows.
    DETAILED_VIEW_LABEL,
    // System information text (e.g. date/time, battery status, "Scanning for
    // devices..." seen in the Bluetooth detailed view, etc).
    SYSTEM_INFO,
    // System information text within a clickable row.
    CLICKABLE_SYSTEM_INFO,
    // Child buttons within rows that have a visible border (e.g. Cast's
    // "Stop", etc).
    BUTTON,
    // Sub text within a row (e.g. user name in user row).
    CAPTION,
  };

  static constexpr double kInactiveIconAlpha = 0.54;

  static SkColor GetIconColor(ColorStyle color_style);

  explicit TrayPopupItemStyle(FontStyle font_style);
  ~TrayPopupItemStyle();

  ColorStyle color_style() const { return color_style_; }

  void set_color_style(ColorStyle color_style) { color_style_ = color_style; }

  FontStyle font_style() const { return font_style_; }

  void set_font_style(FontStyle font_style) { font_style_ = font_style; }

  SkColor GetTextColor() const;

  SkColor GetIconColor() const;

  // Configures a Label as per the style (e.g. color, font).
  void SetupLabel(views::Label* label) const;

 private:
  FontStyle font_style_;

  ColorStyle color_style_;

  DISALLOW_COPY_AND_ASSIGN(TrayPopupItemStyle);
};

}  // namespace ash

#endif  // ASH_SYSTEM_TRAY_TRAY_POPUP_ITEM_STYLE_H_
