// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/browser_process.h"
#include "chrome/browser/signin/token_revoker_test_utils.h"
#include "content/public/test/test_utils.h"
#include "google_apis/gaia/gaia_auth_fetcher.h"
#include "google_apis/gaia/gaia_constants.h"

namespace token_revoker_test_utils {

RefreshTokenRevoker::RefreshTokenRevoker()
    : gaia_fetcher_(this,
                    GaiaConstants::kChromeSource,
                    g_browser_process->system_request_context()) {
}

RefreshTokenRevoker::~RefreshTokenRevoker() {
}

void RefreshTokenRevoker::Revoke(const std::string& token) {
  DVLOG(1) << "Starting RefreshTokenRevoker for token: " << token;
  gaia_fetcher_.StartRevokeOAuth2Token(token);
  message_loop_runner_ = new content::MessageLoopRunner;
  message_loop_runner_->Run();
}

void RefreshTokenRevoker::OnOAuth2RevokeTokenCompleted() {
  DVLOG(1) << "TokenRevoker OnOAuth2RevokeTokenCompleted";
  message_loop_runner_->Quit();
}

}  // namespace token_revoker_test_utils
