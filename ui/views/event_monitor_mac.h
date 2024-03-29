// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_EVENT_MONITOR_MAC_H_
#define UI_VIEWS_EVENT_MONITOR_MAC_H_

#include "base/macros.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/views/event_monitor.h"

namespace views {

class EventMonitorMac : public EventMonitor {
 public:
  EventMonitorMac(ui::EventHandler* event_handler,
                  gfx::NativeWindow target_window);
  ~EventMonitorMac() override;

 private:
  id monitor_;

  DISALLOW_COPY_AND_ASSIGN(EventMonitorMac);
};

}  // namespace views

#endif  // UI_VIEWS_EVENT_MONITOR_MAC_H_
