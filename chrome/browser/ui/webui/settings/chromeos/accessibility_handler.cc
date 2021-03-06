// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/webui/settings/chromeos/accessibility_handler.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/common/extensions/extension_constants.h"
#include "content/public/browser/web_ui.h"
#include "extensions/browser/extension_registry.h"

namespace chromeos {
namespace settings {

AccessibilityHandler::AccessibilityHandler(content::WebUI* webui)
    : profile_(Profile::FromWebUI(webui)) {
}

AccessibilityHandler::~AccessibilityHandler() {}

void AccessibilityHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "showChromeVoxSettings",
      base::Bind(&AccessibilityHandler::HandleShowChromeVoxSettings,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "showSelectToSpeakSettings",
      base::Bind(&AccessibilityHandler::HandleShowSelectToSpeakSettings,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "showSwitchAccessSettings",
      base::Bind(&AccessibilityHandler::HandleShowSwitchAccessSettings,
                 base::Unretained(this)));
}

void AccessibilityHandler::HandleShowChromeVoxSettings(
    const base::ListValue* args) {
  OpenExtensionOptionsPage(extension_misc::kChromeVoxExtensionId);
}

void AccessibilityHandler::HandleShowSelectToSpeakSettings(
    const base::ListValue* args) {
  OpenExtensionOptionsPage(extension_misc::kSelectToSpeakExtensionId);
}

void AccessibilityHandler::HandleShowSwitchAccessSettings(
    const base::ListValue* args) {
  OpenExtensionOptionsPage(extension_misc::kSwitchAccessExtensionId);
}

void AccessibilityHandler::OpenExtensionOptionsPage(const char extension_id[]) {
  const extensions::Extension* extension =
      extensions::ExtensionRegistry::Get(profile_)->GetExtensionById(
          extension_id, extensions::ExtensionRegistry::ENABLED);
  if (!extension)
    return;
  extensions::ExtensionTabUtil::OpenOptionsPage(
      extension,
      chrome::FindBrowserWithWebContents(web_ui()->GetWebContents()));
}

}  // namespace settings
}  // namespace chromeos
