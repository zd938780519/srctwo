// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/macros.h"
#include "base/version.h"
#include "extensions/common/extension.h"
#include "extensions/common/manifest_handlers/shared_module_info.h"
#include "extensions/common/manifest_test.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char kValidImportPath[] =
    "_modules/abcdefghijklmnopabcdefghijklmnop/foo/bar.html";
const char kValidImportPathID[] = "abcdefghijklmnopabcdefghijklmnop";
const char kValidImportPathRelative[] = "foo/bar.html";
const char kInvalidImportPath[] = "_modules/abc/foo.html";
const char kImportId1[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
const char kImportId2[] = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
const char kNoImport[] = "cccccccccccccccccccccccccccccccc";

}  // namespace

namespace extensions {

class SharedModuleManifestTest : public ManifestTest {
};

TEST_F(SharedModuleManifestTest, ExportsAll) {
  ManifestData manifest("shared_module_export.json");

  scoped_refptr<Extension> extension = LoadAndExpectSuccess(manifest);

  EXPECT_TRUE(SharedModuleInfo::IsSharedModule(extension.get()))
      << manifest.name();
  EXPECT_FALSE(SharedModuleInfo::ImportsModules(extension.get()))
      << manifest.name();

  EXPECT_TRUE(SharedModuleInfo::IsExportAllowedByWhitelist(extension.get(),
                  kImportId1)) << manifest.name();
  EXPECT_TRUE(SharedModuleInfo::IsExportAllowedByWhitelist(extension.get(),
                  kImportId2)) << manifest.name();
  EXPECT_FALSE(SharedModuleInfo::IsExportAllowedByWhitelist(extension.get(),
                  kNoImport)) << manifest.name();
}

TEST_F(SharedModuleManifestTest, ExportWhitelistAll) {
  ManifestData manifest("shared_module_export_no_whitelist.json");

  scoped_refptr<Extension> extension = LoadAndExpectSuccess(manifest);

  EXPECT_TRUE(SharedModuleInfo::IsExportAllowedByWhitelist(extension.get(),
                  kImportId1)) << manifest.name();
  EXPECT_TRUE(SharedModuleInfo::IsExportAllowedByWhitelist(extension.get(),
                  kImportId2)) << manifest.name();
  EXPECT_TRUE(SharedModuleInfo::IsExportAllowedByWhitelist(extension.get(),
                  kNoImport)) << manifest.name();
}

TEST_F(SharedModuleManifestTest, ExportParseErrors) {
  Testcase testcases[] = {
    Testcase("shared_module_export_and_import.json",
             "Simultaneous 'import' and 'export' are not allowed."),
    Testcase("shared_module_export_not_dict.json",
             "Invalid value for 'export'."),
    Testcase("shared_module_export_whitelist_item_not_id.json",
             "Invalid value for 'export.whitelist[0]'."),
    Testcase("shared_module_export_whitelist_item_not_string.json",
             "Invalid value for 'export.whitelist[0]'."),
    Testcase("shared_module_export_whitelist_not_list.json",
             "Invalid value for 'export.whitelist'."),
  };
  RunTestcases(testcases, arraysize(testcases), EXPECT_TYPE_ERROR);
}

TEST_F(SharedModuleManifestTest, SharedModuleStaticFunctions) {
  EXPECT_TRUE(SharedModuleInfo::IsImportedPath(kValidImportPath));
  EXPECT_FALSE(SharedModuleInfo::IsImportedPath(kInvalidImportPath));

  std::string id;
  std::string relative;
  SharedModuleInfo::ParseImportedPath(kValidImportPath, &id, &relative);
  EXPECT_EQ(id, kValidImportPathID);
  EXPECT_EQ(relative, kValidImportPathRelative);
}

TEST_F(SharedModuleManifestTest, Import) {
  ManifestData manifest("shared_module_import.json");

  scoped_refptr<Extension> extension = LoadAndExpectSuccess(manifest);

  EXPECT_FALSE(SharedModuleInfo::IsSharedModule(extension.get()))
      << manifest.name();
  EXPECT_TRUE(SharedModuleInfo::ImportsModules(extension.get()))
      << manifest.name();
  const std::vector<SharedModuleInfo::ImportInfo>& imports =
      SharedModuleInfo::GetImports(extension.get());
  ASSERT_EQ(2U, imports.size());
  EXPECT_EQ(imports[0].extension_id, kImportId1);
  EXPECT_EQ(imports[0].minimum_version, "");
  EXPECT_EQ(imports[1].extension_id, kImportId2);
  EXPECT_TRUE(base::Version(imports[1].minimum_version).IsValid());
  EXPECT_TRUE(
      SharedModuleInfo::ImportsExtensionById(extension.get(), kImportId1));
  EXPECT_TRUE(
      SharedModuleInfo::ImportsExtensionById(extension.get(), kImportId2));
  EXPECT_FALSE(
      SharedModuleInfo::ImportsExtensionById(extension.get(), kNoImport));
}

TEST_F(SharedModuleManifestTest, ImportParseErrors) {
  Testcase testcases[] = {
    Testcase("shared_module_import_not_list.json",
             "Invalid value for 'import'."),
    Testcase("shared_module_import_invalid_id.json",
             "Invalid value for 'import[0].id'."),
    Testcase("shared_module_import_invalid_version.json",
             "Invalid value for 'import[0].minimum_version'."),
  };
  RunTestcases(testcases, arraysize(testcases), EXPECT_TYPE_ERROR);
}

}  // namespace extensions
