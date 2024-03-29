// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/layout/FragmentainerIterator.h"

#include "core/layout/LayoutMultiColumnSet.h"

namespace blink {

FragmentainerIterator::FragmentainerIterator(
    const LayoutFlowThread& flow_thread,
    const LayoutRect& physical_bounding_box_in_flow_thread,
    const LayoutRect& clip_rect_in_multicol_container)
    : flow_thread_(flow_thread),
      clip_rect_in_multicol_container_(clip_rect_in_multicol_container),
      current_fragmentainer_group_index_(0) {
  // Put the bounds into flow thread-local coordinates by flipping it first.
  // This is how rectangles typically are represented in layout, i.e. with the
  // block direction coordinate flipped, if writing mode is vertical-rl.
  LayoutRect bounds_in_flow_thread = physical_bounding_box_in_flow_thread;
  flow_thread_.FlipForWritingMode(bounds_in_flow_thread);

  if (flow_thread_.IsHorizontalWritingMode()) {
    logical_top_in_flow_thread_ = bounds_in_flow_thread.Y();
    logical_bottom_in_flow_thread_ = bounds_in_flow_thread.MaxY();
  } else {
    logical_top_in_flow_thread_ = bounds_in_flow_thread.X();
    logical_bottom_in_flow_thread_ = bounds_in_flow_thread.MaxX();
  }

  // Jump to the first interesting column set.
  current_column_set_ = flow_thread.ColumnSetAtBlockOffset(
      logical_top_in_flow_thread_, LayoutBox::kAssociateWithLatterPage);
  if (!current_column_set_) {
    SetAtEnd();
    return;
  }
  // Then find the first interesting fragmentainer group.
  current_fragmentainer_group_index_ =
      current_column_set_->FragmentainerGroupIndexAtFlowThreadOffset(
          logical_top_in_flow_thread_, LayoutBox::kAssociateWithLatterPage);

  // Now find the first and last fragmentainer we're interested in. We'll also
  // clip against the clip rect here. In case the clip rect doesn't intersect
  // with any of the fragmentainers, we have to move on to the next
  // fragmentainer group, and see if we find something there.
  if (!SetFragmentainersOfInterest()) {
    MoveToNextFragmentainerGroup();
    if (AtEnd())
      return;
  }
}

void FragmentainerIterator::Advance() {
  DCHECK(!AtEnd());

  if (current_fragmentainer_index_ < end_fragmentainer_index_) {
    current_fragmentainer_index_++;
  } else {
    // That was the last fragmentainer to visit in this fragmentainer group.
    // Advance to the next group.
    MoveToNextFragmentainerGroup();
    if (AtEnd())
      return;
  }
}

LayoutSize FragmentainerIterator::PaginationOffset() const {
  DCHECK(!AtEnd());
  const MultiColumnFragmentainerGroup& group = CurrentGroup();
  LayoutUnit fragmentainer_logical_top_in_flow_thread =
      group.LogicalTopInFlowThread() +
      current_fragmentainer_index_ * group.ColumnLogicalHeight();
  return group.FlowThreadTranslationAtOffset(
      fragmentainer_logical_top_in_flow_thread,
      LayoutBox::kAssociateWithLatterPage, CoordinateSpaceConversion::kVisual);
}

LayoutRect FragmentainerIterator::ClipRectInFlowThread(
    MultiColumnFragmentainerGroup::ClipRectAxesSelector axes_selector) const {
  DCHECK(!AtEnd());
  LayoutRect clip_rect = CurrentGroup().FlowThreadPortionOverflowRectAt(
      current_fragmentainer_index_, axes_selector);
  flow_thread_.FlipForWritingMode(clip_rect);
  return clip_rect;
}

const MultiColumnFragmentainerGroup& FragmentainerIterator::CurrentGroup()
    const {
  DCHECK(!AtEnd());
  return current_column_set_
      ->FragmentainerGroups()[current_fragmentainer_group_index_];
}

void FragmentainerIterator::MoveToNextFragmentainerGroup() {
  do {
    current_fragmentainer_group_index_++;
    if (current_fragmentainer_group_index_ >=
        current_column_set_->FragmentainerGroups().size()) {
      // That was the last fragmentainer group in this set. Advance to the next.
      current_column_set_ = current_column_set_->NextSiblingMultiColumnSet();
      current_fragmentainer_group_index_ = 0;
      if (!current_column_set_ ||
          current_column_set_->LogicalTopInFlowThread() >=
              logical_bottom_in_flow_thread_) {
        SetAtEnd();
        return;  // No more sets or next set out of range. We're done.
      }
    }
    if (CurrentGroup().LogicalTopInFlowThread() >=
        logical_bottom_in_flow_thread_) {
      // This fragmentainer group doesn't intersect with the range we're
      // interested in. We're done.
      SetAtEnd();
      return;
    }
  } while (!SetFragmentainersOfInterest());
}

bool FragmentainerIterator::SetFragmentainersOfInterest() {
  const MultiColumnFragmentainerGroup& group = CurrentGroup();

  // Figure out the start and end fragmentainers for the block range we're
  // interested in. We might not have to walk the entire fragmentainer group.
  group.ColumnIntervalForBlockRangeInFlowThread(
      logical_top_in_flow_thread_, logical_bottom_in_flow_thread_,
      current_fragmentainer_index_, end_fragmentainer_index_);

  if (HasClipRect()) {
    // Now intersect with the fragmentainers that actually intersect with the
    // visual clip rect, to narrow it down even further. The clip rect needs to
    // be relative to the current fragmentainer group.
    LayoutRect clip_rect = clip_rect_in_multicol_container_;
    LayoutSize offset = group.FlowThreadTranslationAtOffset(
        group.LogicalTopInFlowThread(), LayoutBox::kAssociateWithFormerPage,
        CoordinateSpaceConversion::kVisual);
    clip_rect.Move(-offset);
    unsigned first_fragmentainer_in_clip_rect, last_fragmentainer_in_clip_rect;
    group.ColumnIntervalForVisualRect(clip_rect,
                                      first_fragmentainer_in_clip_rect,
                                      last_fragmentainer_in_clip_rect);
    // If the two fragmentainer intervals are disjoint, there's nothing of
    // interest in this fragmentainer group.
    if (first_fragmentainer_in_clip_rect > end_fragmentainer_index_ ||
        last_fragmentainer_in_clip_rect < current_fragmentainer_index_)
      return false;
    if (current_fragmentainer_index_ < first_fragmentainer_in_clip_rect)
      current_fragmentainer_index_ = first_fragmentainer_in_clip_rect;
    if (end_fragmentainer_index_ > last_fragmentainer_in_clip_rect)
      end_fragmentainer_index_ = last_fragmentainer_in_clip_rect;
  }
  DCHECK_GE(end_fragmentainer_index_, current_fragmentainer_index_);
  return true;
}

}  // namespace blink
