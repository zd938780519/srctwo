// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_TEST_TEST_CONTENT_BROWSER_CLIENT_H_
#define CONTENT_TEST_TEST_CONTENT_BROWSER_CLIENT_H_

#include <string>

#include "base/compiler_specific.h"
#include "base/files/scoped_temp_dir.h"
#include "base/macros.h"
#include "content/public/browser/content_browser_client.h"

namespace content {

// Base for unit tests that need a ContentBrowserClient.
class TestContentBrowserClient : public ContentBrowserClient {
 public:
  TestContentBrowserClient();
  ~TestContentBrowserClient() override;
  base::FilePath GetDefaultDownloadDirectory() override;
  void GetQuotaSettings(
      content::BrowserContext* context,
      content::StoragePartition* partition,
      storage::OptionalQuotaSettingsCallback callback) override;

 private:
  // Temporary directory for GetDefaultDownloadDirectory.
  base::ScopedTempDir download_dir_;

  DISALLOW_COPY_AND_ASSIGN(TestContentBrowserClient);
};

}  // namespace content

#endif  // CONTENT_TEST_TEST_CONTENT_BROWSER_CLIENT_H_
