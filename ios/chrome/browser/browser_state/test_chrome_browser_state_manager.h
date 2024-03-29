// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_BROWSER_STATE_TEST_CHROME_BROWSER_STATE_MANAGER_H_
#define IOS_CHROME_BROWSER_BROWSER_STATE_TEST_CHROME_BROWSER_STATE_MANAGER_H_

#include "base/macros.h"
#include "ios/chrome/browser/browser_state/browser_state_info_cache.h"
#include "ios/chrome/browser/browser_state/chrome_browser_state_manager.h"
#include "ios/chrome/test/ios_chrome_scoped_testing_local_state.h"

namespace ios {
class ChromeBrowserState;
}

// ChromeBrowserStateManager implementation for tests.
class TestChromeBrowserStateManager : public ios::ChromeBrowserStateManager {
 public:
  explicit TestChromeBrowserStateManager(const base::FilePath& user_data_dir);
  ~TestChromeBrowserStateManager() override;

  // ChromeBrowserStateManager:
  ios::ChromeBrowserState* GetLastUsedBrowserState() override;
  ios::ChromeBrowserState* GetBrowserState(const base::FilePath& path) override;
  BrowserStateInfoCache* GetBrowserStateInfoCache() override;
  std::vector<ios::ChromeBrowserState*> GetLoadedBrowserStates() override;

 private:
  IOSChromeScopedTestingLocalState local_state_;
  BrowserStateInfoCache browser_state_info_cache_;

  DISALLOW_COPY_AND_ASSIGN(TestChromeBrowserStateManager);
};

#endif  // IOS_CHROME_BROWSER_BROWSER_STATE_TEST_CHROME_BROWSER_STATE_MANAGER_H_
