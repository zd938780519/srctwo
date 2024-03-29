// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PROFILES_CHROME_BROWSER_MAIN_EXTRA_PARTS_PROFILES_H_
#define CHROME_BROWSER_PROFILES_CHROME_BROWSER_MAIN_EXTRA_PARTS_PROFILES_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "chrome/browser/chrome_browser_main_extra_parts.h"

class ChromeBrowserMainParts;

namespace chrome {
void AddProfilesExtraParts(ChromeBrowserMainParts* main_parts);
}

class ChromeBrowserMainExtraPartsProfiles : public ChromeBrowserMainExtraParts {
 public:
  ChromeBrowserMainExtraPartsProfiles();
  ~ChromeBrowserMainExtraPartsProfiles() override;

  // Instantiates all chrome KeyedService factories, which is
  // especially important for services that should be created at profile
  // creation time as compared to lazily on first access.
  static void EnsureBrowserContextKeyedServiceFactoriesBuilt();

  // Overridden from ChromeBrowserMainExtraParts:
  void PreProfileInit() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(ChromeBrowserMainExtraPartsProfiles);
};

#endif  // CHROME_BROWSER_PROFILES_CHROME_BROWSER_MAIN_EXTRA_PARTS_PROFILES_H_
