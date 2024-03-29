// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/url_pattern_index/ngram_extractor.h"

#include <stdint.h>

#include <string>
#include <vector>

#include "testing/gtest/include/gtest/gtest.h"

namespace url_pattern_index {

namespace {

bool IsSeparatorTrue(char) {
  return true;
}
bool IsSeparatorFalse(char) {
  return false;
}
bool IsSpecialChar(char c) {
  return c == '*' || c == '^';
}

template <typename IntType>
IntType EncodeStringToInteger(const char* data, size_t size) {
  EXPECT_LE(size, sizeof(IntType));

  IntType encoded_string = 0;
  for (size_t i = 0; i < size; ++i) {
    encoded_string = (encoded_string << 8) | static_cast<IntType>(data[i]);
  }
  return encoded_string;
}

}  // namespace

TEST(NGramExtractorTest, EmptyString) {
  const char* kString = "";
  auto extractor = CreateNGramExtractor<3, uint32_t>(kString, IsSpecialChar);
  EXPECT_EQ(extractor.begin(), extractor.end());
}

TEST(NGramExtractorTest, ShortString) {
  const char* kString = "abacab";
  auto extractor = CreateNGramExtractor<7, uint64_t>(kString, IsSeparatorFalse);
  EXPECT_EQ(extractor.begin(), extractor.end());
}

TEST(NGramExtractorTest, ShortPieces) {
  const char* kString = "1**abac*abc*abcd*00";
  auto extractor = CreateNGramExtractor<6, uint64_t>(kString, IsSpecialChar);
  EXPECT_EQ(extractor.begin(), extractor.end());
}

TEST(NGramExtractorTest, IsSeparatorAlwaysTrue) {
  const char* kString = "abacaba";
  auto extractor = CreateNGramExtractor<3, uint32_t>(kString, IsSeparatorTrue);
  EXPECT_EQ(extractor.begin(), extractor.end());
}

TEST(NGramExtractorTest, IsSeparatorAlwaysFalse) {
  const std::string kString = "abacaba123";
  constexpr size_t N = 3;

  std::vector<uint32_t> expected_ngrams;
  for (size_t begin = 0; begin + N <= kString.size(); ++begin) {
    expected_ngrams.push_back(
        EncodeStringToInteger<uint32_t>(kString.data() + begin, N));
  }

  auto extractor = CreateNGramExtractor<N, uint32_t>(kString, IsSeparatorFalse);
  std::vector<uint32_t> actual_ngrams(extractor.begin(), extractor.end());
  EXPECT_EQ(expected_ngrams, actual_ngrams);
}

TEST(NGramExtractorTest, NGramsArePresent) {
  constexpr size_t N = 6;
  const std::string kTestCases[] = {
      "abcdef",   "abacaba",   "*abacaba",
      "abacaba*", "*abacaba*", "*abacaba*abc^1005001*",
  };

  for (const std::string& string : kTestCases) {
    SCOPED_TRACE(testing::Message() << "String: " << string);

    std::vector<uint64_t> expected_ngrams;
    for (size_t begin = 0; begin + N <= string.size(); ++begin) {
      bool is_valid_ngram = true;
      for (size_t i = 0; i < N; ++i) {
        if (IsSpecialChar(string[begin + i])) {
          is_valid_ngram = false;
          break;
        }
      }
      if (is_valid_ngram) {
        expected_ngrams.push_back(
            EncodeStringToInteger<uint64_t>(string.data() + begin, N));
      }
    }

    auto extractor = CreateNGramExtractor<N, uint64_t>(string, IsSpecialChar);
    std::vector<uint64_t> actual_ngrams(extractor.begin(), extractor.end());
    EXPECT_EQ(expected_ngrams, actual_ngrams);
  }
}

}  // namespace url_pattern_index
