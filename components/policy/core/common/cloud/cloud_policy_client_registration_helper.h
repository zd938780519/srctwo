// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_POLICY_CORE_COMMON_CLOUD_CLOUD_POLICY_CLIENT_REGISTRATION_HELPER_H_
#define COMPONENTS_POLICY_CORE_COMMON_CLOUD_CLOUD_POLICY_CLIENT_REGISTRATION_HELPER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "build/build_config.h"
#include "components/policy/core/common/cloud/cloud_policy_client.h"
#include "components/policy/core/common/cloud/user_info_fetcher.h"
#include "components/policy/policy_export.h"
#include "components/policy/proto/device_management_backend.pb.h"

class OAuth2TokenService;

namespace net {
class URLRequestContextGetter;
}

namespace policy {

// Helper class that registers a CloudPolicyClient. It fetches an OAuth2 token
// for the DM service if needed, and checks with Gaia if the account has policy
// management enabled.
class POLICY_EXPORT CloudPolicyClientRegistrationHelper
    : public UserInfoFetcher::Delegate,
      public CloudPolicyClient::Observer {
 public:
  // |context| and |client| are not owned and must outlive this object.
  CloudPolicyClientRegistrationHelper(
      CloudPolicyClient* client,
      enterprise_management::DeviceRegisterRequest::Type registration_type);
  ~CloudPolicyClientRegistrationHelper() override;

  // Starts the client registration process. This version uses the
  // supplied OAuth2TokenService to mint the new token for the userinfo
  // and DM services, using the |account_id|.
  // |callback| is invoked when the registration is complete.
  void StartRegistration(
      OAuth2TokenService* token_service,
      const std::string& account_id,
      const base::Closure& callback);

#if !defined(OS_ANDROID)
  // Starts the client registration process. The |login_refresh_token| is used
  // to mint a new token for the userinfo and DM services.
  // |callback| is invoked when the registration is complete.
  void StartRegistrationWithLoginToken(const std::string& login_refresh_token,
                                       const base::Closure& callback);

  // Returns the scopes required for policy client registration.
  static std::vector<std::string> GetScopes();
#endif

 private:
  class TokenServiceHelper;
#if !defined(OS_ANDROID)
  class LoginTokenHelper;
#endif

  void OnTokenFetched(const std::string& oauth_access_token);

  // UserInfoFetcher::Delegate implementation:
  void OnGetUserInfoSuccess(const base::DictionaryValue* response) override;
  void OnGetUserInfoFailure(const GoogleServiceAuthError& error) override;

  // CloudPolicyClient::Observer implementation:
  void OnPolicyFetched(CloudPolicyClient* client) override;
  void OnRegistrationStateChanged(CloudPolicyClient* client) override;
  void OnClientError(CloudPolicyClient* client) override;

  // Invoked when the registration request has been completed.
  void RequestCompleted();

  // Internal helper class that uses OAuth2TokenService to fetch an OAuth
  // access token. On desktop, this is only used after the user has signed in -
  // desktop platforms use LoginTokenHelper for policy fetches performed before
  // signin is complete.
  std::unique_ptr<TokenServiceHelper> token_service_helper_;

#if !defined(OS_ANDROID)
  // Special desktop-only helper to fetch an OAuth access token prior to
  // the completion of signin. Not used on Android since all token fetching
  // is done via OAuth2TokenService.
  std::unique_ptr<LoginTokenHelper> login_token_helper_;
#endif

  // Helper class for fetching information from GAIA about the currently
  // signed-in user.
  std::unique_ptr<UserInfoFetcher> user_info_fetcher_;

  // Access token used to register the CloudPolicyClient and also access
  // GAIA to get information about the signed in user.
  std::string oauth_access_token_;

  scoped_refptr<net::URLRequestContextGetter> context_;
  CloudPolicyClient* client_;
  enterprise_management::DeviceRegisterRequest::Type registration_type_;
  base::Closure callback_;

  DISALLOW_COPY_AND_ASSIGN(CloudPolicyClientRegistrationHelper);
};

}  // namespace policy

#endif  // COMPONENTS_POLICY_CORE_COMMON_CLOUD_CLOUD_POLICY_CLIENT_REGISTRATION_HELPER_H_
