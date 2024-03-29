// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/omnibox/browser/history_match.h"

#include "base/logging.h"

namespace history {

HistoryMatch::HistoryMatch()
    : url_info(),
      input_location(base::string16::npos),
      match_in_scheme(false),
      match_in_subdomain(false),
      match_after_host(false),
      innermost_match(true) {}

bool HistoryMatch::EqualsGURL(const HistoryMatch& h, const GURL& url) {
  return h.url_info.url() == url;
}

bool HistoryMatch::IsHostOnly() const {
  const GURL& gurl = url_info.url();
  DCHECK(gurl.is_valid());
  return (!gurl.has_path() || (gurl.path_piece() == "/")) &&
         !gurl.has_query() && !gurl.has_ref();
}

}  // namespace history
