// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/shell_app_window_client.h"

#include <vector>

#include "extensions/browser/app_window/app_window.h"
#include "extensions/shell/browser/shell_app_delegate.h"

namespace extensions {

ShellAppWindowClient::ShellAppWindowClient() {
}

ShellAppWindowClient::~ShellAppWindowClient() {
}

AppWindow* ShellAppWindowClient::CreateAppWindow(
    content::BrowserContext* context,
    const Extension* extension) {
  return new AppWindow(context, new ShellAppDelegate, extension);
}

AppWindow* ShellAppWindowClient::CreateAppWindowForLockScreenAction(
    content::BrowserContext* context,
    const Extension* extension,
    api::app_runtime::ActionType action) {
  return nullptr;
}

void ShellAppWindowClient::OpenDevToolsWindow(
    content::WebContents* web_contents,
    const base::Closure& callback) {
  NOTIMPLEMENTED();
}

bool ShellAppWindowClient::IsCurrentChannelOlderThanDev() {
  return false;
}

}  // namespace extensions
