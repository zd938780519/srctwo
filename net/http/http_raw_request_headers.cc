// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/http/http_raw_request_headers.h"

namespace net {

HttpRawRequestHeaders::HttpRawRequestHeaders() = default;
HttpRawRequestHeaders::HttpRawRequestHeaders(HttpRawRequestHeaders&&) = default;
HttpRawRequestHeaders& HttpRawRequestHeaders::operator=(
    HttpRawRequestHeaders&&) = default;
HttpRawRequestHeaders::~HttpRawRequestHeaders() = default;

void HttpRawRequestHeaders::Add(base::StringPiece key,
                                base::StringPiece value) {
  headers_.emplace_back(key.as_string(), value.as_string());
}

bool HttpRawRequestHeaders::FindHeaderForTest(base::StringPiece key,
                                              std::string* value) const {
  for (const auto& entry : headers_) {
    if (entry.first == key) {
      *value = entry.second;
      return true;
    }
  }
  return false;
}

}  // namespace net
