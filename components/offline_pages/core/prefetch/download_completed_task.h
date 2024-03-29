// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_OFFLINE_PAGES_CORE_PREFETCH_DOWNLOAD_COMPLETED_TASK_H_
#define COMPONENTS_OFFLINE_PAGES_CORE_PREFETCH_DOWNLOAD_COMPLETED_TASK_H_

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "components/offline_pages/core/prefetch/prefetch_types.h"
#include "components/offline_pages/core/task.h"

namespace offline_pages {
class PrefetchDispatcher;
class PrefetchStore;

// Task that responses to the completed download.
class DownloadCompletedTask : public Task {
 public:
  DownloadCompletedTask(PrefetchDispatcher* prefetch_dispatcher,
                        PrefetchStore* prefetch_store,
                        const PrefetchDownloadResult& download_result);
  ~DownloadCompletedTask() override;

  void Run() override;

 private:
  void OnPrefetchItemUpdated(bool successful_download, bool row_was_updated);

  PrefetchDispatcher* prefetch_dispatcher_;  // Outlives this class.
  PrefetchStore* prefetch_store_;  // Outlives this class.
  PrefetchDownloadResult download_result_;

  base::WeakPtrFactory<DownloadCompletedTask> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(DownloadCompletedTask);
};

}  // namespace offline_pages

#endif  // COMPONENTS_OFFLINE_PAGES_CORE_PREFETCH_DOWNLOAD_COMPLETED_TASK_H_
