// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_FONT_PREF_CHANGE_NOTIFIER_FACTORY_H_
#define CHROME_BROWSER_FONT_PREF_CHANGE_NOTIFIER_FACTORY_H_

#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

class FontPrefChangeNotifier;
class Profile;

// Keyed service factory for a FontPrefChangeNotifier.
class FontPrefChangeNotifierFactory : public BrowserContextKeyedServiceFactory {
 public:
  static FontPrefChangeNotifier* GetForProfile(Profile* profile);

  static FontPrefChangeNotifierFactory* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<FontPrefChangeNotifierFactory>;

  FontPrefChangeNotifierFactory();
  ~FontPrefChangeNotifierFactory() override;

  // BrowserContextKeyedServiceFactory overrides:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
};

#endif  // CHROME_BROWSER_FONT_PREF_CHANGE_NOTIFIER_FACTORY_H_
