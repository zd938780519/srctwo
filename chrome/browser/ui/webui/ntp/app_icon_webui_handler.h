// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_NTP_APP_ICON_WEBUI_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_NTP_APP_ICON_WEBUI_HANDLER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/task/cancelable_task_tracker.h"
#include "chrome/browser/extensions/extension_icon_manager.h"
#include "content/public/browser/web_ui_message_handler.h"

namespace base {
class ListValue;
}

// Handles requests for app icon "dominant" color, i.e. the stripe that appears
// at the bottom of an app tile that has been generated from a link.
class AppIconWebUIHandler : public content::WebUIMessageHandler,
                            public ExtensionIconManager::Observer {
 public:
  AppIconWebUIHandler();
  ~AppIconWebUIHandler() override;

  // WebUIMessageHandler
  void RegisterMessages() override;

  // The sole argument is the extension ID.
  void HandleGetAppIconDominantColor(const base::ListValue* args);

  // ExtensionIconManager::Observer:
  void OnImageLoaded(const std::string& extension_id) override;

 private:
  base::CancelableTaskTracker cancelable_task_tracker_;

  // Manage retrieval of icons from apps.
  ExtensionIconManager extension_icon_manager_;

  DISALLOW_COPY_AND_ASSIGN(AppIconWebUIHandler);
};

#endif  // CHROME_BROWSER_UI_WEBUI_NTP_APP_ICON_WEBUI_HANDLER_H_
