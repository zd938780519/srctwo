// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BoundaryEventDispatcher_h
#define BoundaryEventDispatcher_h

#include "core/dom/events/EventTarget.h"

namespace blink {

// This class contains the common logic of finding related node in a boundary
// crossing action and sending boundary events. The subclasses of this class
// must define what events should be sent in every case.
class BoundaryEventDispatcher {
  STACK_ALLOCATED();

 public:
  BoundaryEventDispatcher() {}
  virtual ~BoundaryEventDispatcher() {}

  void SendBoundaryEvents(EventTarget* exited_target,
                          EventTarget* entered_target);

 protected:
  virtual void DispatchOut(EventTarget*, EventTarget* related_target) = 0;
  virtual void DispatchOver(EventTarget*, EventTarget* related_target) = 0;
  virtual void DispatchLeave(EventTarget*,
                             EventTarget* related_target,
                             bool check_for_listener) = 0;
  virtual void DispatchEnter(EventTarget*,
                             EventTarget* related_target,
                             bool check_for_listener) = 0;
  virtual AtomicString GetLeaveEvent() = 0;
  virtual AtomicString GetEnterEvent() = 0;
};

}  // namespace blink

#endif  // BoundaryEventDispatcher_h
