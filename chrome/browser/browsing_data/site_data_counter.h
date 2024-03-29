// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_BROWSING_DATA_SITE_DATA_COUNTER_H_
#define CHROME_BROWSER_BROWSING_DATA_SITE_DATA_COUNTER_H_

#include "base/memory/weak_ptr.h"
#include "components/browsing_data/core/counters/browsing_data_counter.h"

class Profile;

class SiteDataCounter : public browsing_data::BrowsingDataCounter {
 public:
  explicit SiteDataCounter(Profile* profile);
  ~SiteDataCounter() override;

  const char* GetPrefName() const override;

 private:
  void Count() override;
  void Done(int origin_count);

  Profile* profile_;
  base::WeakPtrFactory<SiteDataCounter> weak_ptr_factory_;
};

#endif  // CHROME_BROWSER_BROWSING_DATA_SITE_DATA_COUNTER_H_
