// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ACCESSIBILITY_ACCESSIBILITY_PERMISSION_CONTEXT_H_
#define CHROME_BROWSER_ACCESSIBILITY_ACCESSIBILITY_PERMISSION_CONTEXT_H_

#include "base/macros.h"
#include "chrome/browser/permissions/permission_context_base.h"

class AccessibilityPermissionContext : public PermissionContextBase {
 public:
  explicit AccessibilityPermissionContext(Profile* profile);
  ~AccessibilityPermissionContext() override;

 private:
  // PermissionContextBase:
  bool IsRestrictedToSecureOrigins() const override;

  DISALLOW_COPY_AND_ASSIGN(AccessibilityPermissionContext);
};

#endif  // CHROME_BROWSER_ACCESSIBILITY_ACCESSIBILITY_PERMISSION_CONTEXT_H_
