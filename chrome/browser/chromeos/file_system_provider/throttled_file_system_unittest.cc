// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/file_system_provider/throttled_file_system.h"

#include <stddef.h>

#include <memory>
#include <vector>

#include "base/files/file.h"
#include "base/memory/ptr_util.h"
#include "base/run_loop.h"
#include "chrome/browser/chromeos/file_system_provider/abort_callback.h"
#include "chrome/browser/chromeos/file_system_provider/fake_provided_file_system.h"
#include "chrome/browser/chromeos/file_system_provider/provided_file_system_info.h"
#include "chrome/common/extensions/api/file_system_provider_capabilities/file_system_provider_capabilities_handler.h"
#include "content/public/test/test_browser_thread_bundle.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace chromeos {
namespace file_system_provider {
namespace {

const char kExtensionId[] = "mbflcebpggnecokmikipoihdbecnjfoj";
const char kFileSystemId[] = "camera-pictures";
const char kDisplayName[] = "Camera Pictures";

typedef std::vector<base::File::Error> StatusLog;
typedef std::vector<std::pair<int, base::File::Error>> OpenLog;

// Writes a |result| to the |log| vector for a status callback.
void LogStatus(StatusLog* log, base::File::Error result) {
  log->push_back(result);
}

// Writes a |result| to the |log| vector for opening a file.
void LogOpen(OpenLog* log, int handle, base::File::Error result) {
  log->push_back(std::make_pair(handle, result));
}

}  // namespace

class FileSystemProviderThrottledFileSystemTest : public testing::Test {
 protected:
  FileSystemProviderThrottledFileSystemTest() {}
  ~FileSystemProviderThrottledFileSystemTest() override {}

  void SetUp() override {}

  // Initializes the throttled file system with |limit| number of opened files
  // at once. If 0, then no limit.
  void SetUpFileSystem(size_t limit) {
    MountOptions options(kFileSystemId, kDisplayName);
    options.opened_files_limit = limit;

    ProvidedFileSystemInfo file_system_info(
        kExtensionId, options, base::FilePath() /* mount_path */,
        false /* configurable */, true /* watchable */,
        extensions::SOURCE_FILE);

    file_system_.reset(new ThrottledFileSystem(
        base::MakeUnique<FakeProvidedFileSystem>(file_system_info)));
  }

  content::TestBrowserThreadBundle thread_bundle_;
  std::unique_ptr<ThrottledFileSystem> file_system_;
};

TEST_F(FileSystemProviderThrottledFileSystemTest, OpenFile_LimitedToOneAtOnce) {
  SetUpFileSystem(1);

  OpenLog first_open_log;
  file_system_->OpenFile(base::FilePath(kFakeFilePath), OPEN_FILE_MODE_READ,
                         base::Bind(&LogOpen, &first_open_log));

  OpenLog second_open_log;
  file_system_->OpenFile(base::FilePath(kFakeFilePath), OPEN_FILE_MODE_READ,
                         base::Bind(&LogOpen, &second_open_log));

  base::RunLoop().RunUntilIdle();

  ASSERT_EQ(1u, first_open_log.size());
  EXPECT_EQ(base::File::FILE_OK, first_open_log[0].second);
  EXPECT_EQ(0u, second_open_log.size());

  // Close the first file.
  StatusLog close_log;
  file_system_->CloseFile(first_open_log[0].first,
                          base::Bind(&LogStatus, &close_log));

  base::RunLoop().RunUntilIdle();
  ASSERT_EQ(1u, close_log.size());
  EXPECT_EQ(base::File::FILE_OK, close_log[0]);

  // The second enqueued file should be opened.
  ASSERT_EQ(1u, first_open_log.size());
  EXPECT_EQ(base::File::FILE_OK, first_open_log[0].second);
  ASSERT_EQ(1u, second_open_log.size());
  EXPECT_EQ(base::File::FILE_OK, second_open_log[0].second);
}

TEST_F(FileSystemProviderThrottledFileSystemTest, OpenFile_NoLimit) {
  SetUpFileSystem(0);  // No limit.

  OpenLog first_open_log;
  file_system_->OpenFile(base::FilePath(kFakeFilePath), OPEN_FILE_MODE_READ,
                         base::Bind(&LogOpen, &first_open_log));

  OpenLog second_open_log;
  file_system_->OpenFile(base::FilePath(kFakeFilePath), OPEN_FILE_MODE_READ,
                         base::Bind(&LogOpen, &second_open_log));

  base::RunLoop().RunUntilIdle();

  ASSERT_EQ(1u, first_open_log.size());
  EXPECT_EQ(base::File::FILE_OK, first_open_log[0].second);
  ASSERT_EQ(1u, second_open_log.size());
  EXPECT_EQ(base::File::FILE_OK, second_open_log[0].second);

  // Close files.
  StatusLog first_close_log;
  file_system_->CloseFile(first_open_log[0].first,
                          base::Bind(&LogStatus, &first_close_log));

  StatusLog second_close_log;
  file_system_->CloseFile(second_open_log[0].first,
                          base::Bind(&LogStatus, &second_close_log));

  base::RunLoop().RunUntilIdle();
  ASSERT_EQ(1u, first_close_log.size());
  EXPECT_EQ(base::File::FILE_OK, first_close_log[0]);
  ASSERT_EQ(1u, second_close_log.size());
  EXPECT_EQ(base::File::FILE_OK, second_close_log[0]);

  // Confirm, that files are not opened again.
  EXPECT_EQ(1u, first_open_log.size());
  EXPECT_EQ(1u, second_open_log.size());
}

TEST_F(FileSystemProviderThrottledFileSystemTest, AbortAfterRun) {
  SetUpFileSystem(1);

  OpenLog first_open_log;
  AbortCallback abort_callback =
      file_system_->OpenFile(base::FilePath(kFakeFilePath), OPEN_FILE_MODE_READ,
                             base::Bind(&LogOpen, &first_open_log));

  OpenLog second_open_log;
  file_system_->OpenFile(base::FilePath(kFakeFilePath), OPEN_FILE_MODE_READ,
                         base::Bind(&LogOpen, &second_open_log));

  base::RunLoop().RunUntilIdle();

  ASSERT_EQ(1u, first_open_log.size());
  EXPECT_EQ(base::File::FILE_OK, first_open_log[0].second);
  EXPECT_EQ(0u, second_open_log.size());
}

}  // namespace file_system_provider
}  // namespace chromeos
