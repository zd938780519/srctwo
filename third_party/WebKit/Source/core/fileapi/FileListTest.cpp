// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fileapi/FileList.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace blink {

TEST(FileListTest, pathsForUserVisibleFiles) {
  FileList* const file_list = FileList::Create();

  // Native file.
  file_list->Append(File::Create("/native/path"));

  // Blob file.
  const RefPtr<BlobDataHandle> blob_data_handle = BlobDataHandle::Create();
  file_list->Append(File::Create("name", 0.0, blob_data_handle));

  // User visible snapshot file.
  {
    FileMetadata metadata;
    metadata.platform_path = "/native/visible/snapshot";
    file_list->Append(
        File::CreateForFileSystemFile("name", metadata, File::kIsUserVisible));
  }

  // Not user visible snapshot file.
  {
    FileMetadata metadata;
    metadata.platform_path = "/native/not-visible/snapshot";
    file_list->Append(File::CreateForFileSystemFile("name", metadata,
                                                    File::kIsNotUserVisible));
  }

  // User visible file system URL file.
  {
    KURL url(
        ParsedURLStringTag(),
        "filesystem:http://example.com/isolated/hash/visible-non-native-file");
    file_list->Append(File::CreateForFileSystemFile(url, FileMetadata(),
                                                    File::kIsUserVisible));
  }

  // Not user visible file system URL file.
  {
    KURL url(ParsedURLStringTag(),
             "filesystem:http://example.com/isolated/hash/"
             "not-visible-non-native-file");
    file_list->Append(File::CreateForFileSystemFile(url, FileMetadata(),
                                                    File::kIsNotUserVisible));
  }

  Vector<String> paths = file_list->PathsForUserVisibleFiles();

  ASSERT_EQ(3u, paths.size());
  EXPECT_EQ("/native/path", paths[0]);
  EXPECT_EQ("/native/visible/snapshot", paths[1]);
  EXPECT_EQ("visible-non-native-file", paths[2])
      << "Files not backed by a native file should return name.";
}

}  // namespace blink
