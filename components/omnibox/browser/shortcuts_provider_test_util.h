// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "base/memory/ref_counted.h"
#include "components/omnibox/browser/autocomplete_match.h"

class ShortcutsBackend;
class ShortcutsProvider;

using ExpectedURLAndAllowedToBeDefault = std::pair<std::string, bool>;

struct TestShortcutData {
  TestShortcutData(std::string guid,
                   std::string text,
                   std::string fill_into_edit,
                   std::string destination_url,
                   std::string contents,
                   std::string contents_class,
                   std::string description,
                   std::string description_class,
                   ui::PageTransition transition,
                   AutocompleteMatch::Type type,
                   std::string keyword,
                   int days_from_now,
                   int number_of_hits);
  ~TestShortcutData();

  std::string guid;
  std::string text;
  std::string fill_into_edit;
  std::string destination_url;
  std::string contents;
  std::string contents_class;
  std::string description;
  std::string description_class;
  ui::PageTransition transition;
  AutocompleteMatch::Type type;
  std::string keyword;
  int days_from_now;
  int number_of_hits;
};

// Fills test data into the shortcuts backend.
void PopulateShortcutsBackendWithTestData(
    scoped_refptr<ShortcutsBackend> backend,
    TestShortcutData* db,
    size_t db_size);

// Runs an autocomplete query on |text| with the provided
// |prevent_inline_autocomplete| setting and checks to see that the returned
// results' destination URLs match those provided. |expected_urls| does not
// need to be in sorted order, but |expected_top_result| should be the top
// match, and it should have inline autocompletion
// |top_result_inline_autocompletion|.
void RunShortcutsProviderTest(
    scoped_refptr<ShortcutsProvider> provider,
    const base::string16 text,
    bool prevent_inline_autocomplete,
    const std::vector<ExpectedURLAndAllowedToBeDefault>& expected_urls,
    std::string expected_top_result,
    base::string16 top_result_inline_autocompletion);
