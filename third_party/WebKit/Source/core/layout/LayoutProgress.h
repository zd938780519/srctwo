/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef LayoutProgress_h
#define LayoutProgress_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class HTMLProgressElement;

class CORE_EXPORT LayoutProgress final : public LayoutBlockFlow {
 public:
  explicit LayoutProgress(HTMLProgressElement*);
  ~LayoutProgress() override;

  double GetPosition() const { return position_; }
  double AnimationProgress() const;

  bool IsDeterminate() const;
  void UpdateFromElement() override;

  HTMLProgressElement* ProgressElement() const;

  const char* GetName() const override { return "LayoutProgress"; }

 protected:
  void WillBeDestroyed() override;

  bool IsAnimating() const;
  bool IsAnimationTimerActive() const;

 private:
  bool IsOfType(LayoutObjectType type) const override {
    return type == kLayoutObjectProgress || LayoutBlockFlow::IsOfType(type);
  }

  void AnimationTimerFired(TimerBase*);
  void UpdateAnimationState();

  double position_;
  double animation_start_time_;
  double animation_repeat_interval_;
  double animation_duration_;
  bool animating_;
  TaskRunnerTimer<LayoutProgress> animation_timer_;

  friend class LayoutProgressTest;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutProgress, IsProgress());

}  // namespace blink

#endif  // LayoutProgress_h
