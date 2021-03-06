// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "chrome/browser/chromeos/arc/test/arc_data_removed_waiter.h"

#include "base/memory/ptr_util.h"
#include "base/run_loop.h"

namespace arc {

ArcDataRemovedWaiter::ArcDataRemovedWaiter() {
  DCHECK(ArcSessionManager::Get());
  ArcSessionManager::Get()->AddObserver(this);
}

ArcDataRemovedWaiter::~ArcDataRemovedWaiter() {
  ArcSessionManager::Get()->RemoveObserver(this);
}

void ArcDataRemovedWaiter::Wait() {
  run_loop_ = base::MakeUnique<base::RunLoop>();
  run_loop_->Run();
  run_loop_.reset();
}

void ArcDataRemovedWaiter::OnArcDataRemoved() {
  if (!run_loop_)
    return;
  run_loop_->Quit();
}

}  // namespace arc
