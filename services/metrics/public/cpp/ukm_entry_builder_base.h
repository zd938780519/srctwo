// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_METRICS_PUBLIC_CPP_UKM_ENTRY_BUILDER_BASE_H
#define SERVICES_METRICS_PUBLIC_CPP_UKM_ENTRY_BUILDER_BASE_H

#include <string>

#include "base/macros.h"
#include "services/metrics/public/cpp/metrics_export.h"
#include "services/metrics/public/cpp/ukm_recorder.h"
#include "services/metrics/public/interfaces/ukm_interface.mojom.h"

namespace ukm {

namespace internal {

// A base class for generated UkmEntry builder objects.
// This class should not be used directly.
class METRICS_EXPORT UkmEntryBuilderBase {
 public:
  virtual ~UkmEntryBuilderBase();

  // Records the complete entry into the recorder.  If recorder is null, the
  // entry is simply discarded.
  void Record(UkmRecorder* recorder);

 protected:
  UkmEntryBuilderBase(ukm::SourceId source_id, uint64_t event_hash);
  // Add metric to the entry. A metric contains a metric hash and value.
  void AddMetric(uint64_t metric_hash, int64_t value);

 private:
  mojom::UkmEntryPtr entry_;

  DISALLOW_COPY_AND_ASSIGN(UkmEntryBuilderBase);
};

}  // namespace internal

}  // namespace ukm

#endif  // SERVICES_METRICS_PUBLIC_CPP_UKM_ENTRY_BUILDER_BASE_H
