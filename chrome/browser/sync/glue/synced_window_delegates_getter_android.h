// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_SYNC_GLUE_SYNCED_WINDOW_DELEGATES_GETTER_ANDROID_H_
#define CHROME_BROWSER_SYNC_GLUE_SYNCED_WINDOW_DELEGATES_GETTER_ANDROID_H_

#include <set>

#include "base/macros.h"
#include "components/sync_sessions/synced_window_delegates_getter.h"

namespace sync_sessions {
class SyncedWindowDelegate;
}

namespace browser_sync {

// This class defines how to access SyncedWindowDelegates on Android.
class SyncedWindowDelegatesGetterAndroid
    : public sync_sessions::SyncedWindowDelegatesGetter {
 public:
  SyncedWindowDelegatesGetterAndroid();
  ~SyncedWindowDelegatesGetterAndroid() override;

  // SyncedWindowDelegatesGetter implementation
  SyncedWindowDelegateMap GetSyncedWindowDelegates() override;
  const sync_sessions::SyncedWindowDelegate* FindById(
      SessionID::id_type id) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(SyncedWindowDelegatesGetterAndroid);
};

}  // namespace browser_sync

#endif  // CHROME_BROWSER_SYNC_GLUE_SYNCED_WINDOW_DELEGATES_GETTER_ANDROID_H_
