// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/timing/PerformanceObserverEntryList.h"

#include <algorithm>
#include "core/timing/PerformanceEntry.h"
#include "platform/wtf/StdLibExtras.h"

namespace blink {

PerformanceObserverEntryList::PerformanceObserverEntryList(
    const PerformanceEntryVector& entry_vector)
    : performance_entries_(entry_vector) {}

PerformanceObserverEntryList::~PerformanceObserverEntryList() {}

PerformanceEntryVector PerformanceObserverEntryList::getEntries() const {
  PerformanceEntryVector entries;

  entries.AppendVector(performance_entries_);

  std::sort(entries.begin(), entries.end(),
            PerformanceEntry::StartTimeCompareLessThan);
  return entries;
}

PerformanceEntryVector PerformanceObserverEntryList::getEntriesByType(
    const String& entry_type) {
  PerformanceEntryVector entries;
  PerformanceEntry::EntryType type =
      PerformanceEntry::ToEntryTypeEnum(entry_type);

  if (type == PerformanceEntry::kInvalid)
    return entries;

  for (const auto& entry : performance_entries_) {
    if (entry->EntryTypeEnum() == type) {
      entries.push_back(entry);
    }
  }

  std::sort(entries.begin(), entries.end(),
            PerformanceEntry::StartTimeCompareLessThan);
  return entries;
}

PerformanceEntryVector PerformanceObserverEntryList::getEntriesByName(
    const String& name,
    const String& entry_type) {
  PerformanceEntryVector entries;
  PerformanceEntry::EntryType type =
      PerformanceEntry::ToEntryTypeEnum(entry_type);

  if (!entry_type.IsNull() && type == PerformanceEntry::kInvalid)
    return entries;

  for (const auto& entry : performance_entries_) {
    if (entry->name() == name &&
        (entry_type.IsNull() || type == entry->EntryTypeEnum())) {
      entries.push_back(entry);
    }
  }

  std::sort(entries.begin(), entries.end(),
            PerformanceEntry::StartTimeCompareLessThan);
  return entries;
}

DEFINE_TRACE(PerformanceObserverEntryList) {
  visitor->Trace(performance_entries_);
}

}  // namespace blink
