// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_CHROMEOS_LOGIN_ACTIVE_DIRECTORY_PASSWORD_CHANGE_SCREEN_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_CHROMEOS_LOGIN_ACTIVE_DIRECTORY_PASSWORD_CHANGE_SCREEN_HANDLER_H_

#include <string>

#include "base/macros.h"
#include "chrome/browser/ui/webui/chromeos/login/base_screen_handler.h"
#include "chromeos/login/auth/authpolicy_login_helper.h"

namespace authpolicy {
class ActiveDirectoryAccountInfo;
}

namespace chromeos {

class CoreOobeView;
class Key;
class SigninScreenHandlerDelegate;

// A class that handles WebUI hooks in Active Directory password change  screen.
class ActiveDirectoryPasswordChangeScreenHandler : public BaseScreenHandler {
 public:
  explicit ActiveDirectoryPasswordChangeScreenHandler(
      CoreOobeView* core_oobe_view);
  ~ActiveDirectoryPasswordChangeScreenHandler() override;

  // BaseScreenHandler implementation:
  void DeclareLocalizedValues(
      ::login::LocalizedValuesBuilder* builder) override;
  void Initialize() override;

  // WebUIMessageHandler implementation:
  void RegisterMessages() override;

  // WebUI message handlers.
  void HandleComplete(const std::string& username,
                      const std::string& old_password,
                      const std::string& new_password);
  void HandleCancel();

  // Shows the password change screen for |username|. Uses |delegate| to
  // compelete authentication process.
  void ShowScreen(const std::string& username,
                  SigninScreenHandlerDelegate* delegate);

 private:
  // Shows the screen with the error message corresponding to |error|.
  void ShowScreenWithError(int error);

  // Callback called by AuthPolicyLoginHelper::AuthenticateUser with results and
  // error code. (see AuthPolicyLoginHelper::AuthenticateUser)
  void OnAuthFinished(
      const std::string& username,
      const Key& key,
      authpolicy::ErrorType error,
      const authpolicy::ActiveDirectoryAccountInfo& account_info);

  // Helper to call AuthPolicyClient and cancel calls if needed. Used to change
  // password on the Active Directory server.
  std::unique_ptr<AuthPolicyLoginHelper> authpolicy_login_helper_;

  // Non-owned. Used to complete authentication process.
  SigninScreenHandlerDelegate* delegate_ = nullptr;

  // Non-owned. Used to display signin error.
  CoreOobeView* core_oobe_view_ = nullptr;

  base::WeakPtrFactory<ActiveDirectoryPasswordChangeScreenHandler>
      weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(ActiveDirectoryPasswordChangeScreenHandler);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_UI_WEBUI_CHROMEOS_LOGIN_ACTIVE_DIRECTORY_PASSWORD_CHANGE_SCREEN_HANDLER_H_
