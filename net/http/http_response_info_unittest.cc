// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/http/http_response_info.h"

#include "base/pickle.h"
#include "net/cert/signed_certificate_timestamp.h"
#include "net/cert/signed_certificate_timestamp_and_status.h"
#include "net/http/http_response_headers.h"
#include "net/ssl/ssl_connection_status_flags.h"
#include "net/test/cert_test_util.h"
#include "net/test/ct_test_util.h"
#include "net/test/test_data_directory.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace net {

namespace {

class HttpResponseInfoTest : public testing::Test {
 protected:
  void SetUp() override {
    response_info_.headers = new HttpResponseHeaders("");
  }

  void PickleAndRestore(const HttpResponseInfo& response_info,
                        HttpResponseInfo* restored_response_info) const {
    base::Pickle pickle;
    response_info.Persist(&pickle, false, false);
    bool truncated = false;
    EXPECT_TRUE(restored_response_info->InitFromPickle(pickle, &truncated));
  }

  HttpResponseInfo response_info_;
};

TEST_F(HttpResponseInfoTest, UnusedSincePrefetchDefault) {
  EXPECT_FALSE(response_info_.unused_since_prefetch);
}

TEST_F(HttpResponseInfoTest, UnusedSincePrefetchCopy) {
  response_info_.unused_since_prefetch = true;
  HttpResponseInfo response_info_clone(response_info_);
  EXPECT_TRUE(response_info_clone.unused_since_prefetch);
}

TEST_F(HttpResponseInfoTest, UnusedSincePrefetchPersistFalse) {
  HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_FALSE(restored_response_info.unused_since_prefetch);
}

TEST_F(HttpResponseInfoTest, UnusedSincePrefetchPersistTrue) {
  response_info_.unused_since_prefetch = true;
  HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_TRUE(restored_response_info.unused_since_prefetch);
}

TEST_F(HttpResponseInfoTest, PKPBypassPersistTrue) {
  response_info_.ssl_info.pkp_bypassed = true;
  HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_TRUE(restored_response_info.ssl_info.pkp_bypassed);
}

TEST_F(HttpResponseInfoTest, PKPBypassPersistFalse) {
  response_info_.ssl_info.pkp_bypassed = false;
  HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_FALSE(restored_response_info.ssl_info.pkp_bypassed);
}

TEST_F(HttpResponseInfoTest, FailsInitFromPickleWithInvalidSCTStatus) {
  // A valid certificate is needed for ssl_info.is_valid() to be true
  // so that the SCTs would be serialized.
  response_info_.ssl_info.cert =
      ImportCertFromFile(GetTestCertsDirectory(), "ok_cert.pem");

  scoped_refptr<ct::SignedCertificateTimestamp> sct;
  ct::GetX509CertSCT(&sct);

  response_info_.ssl_info.signed_certificate_timestamps.push_back(
      SignedCertificateTimestampAndStatus(
          sct, ct::SCTVerifyStatus::SCT_STATUS_LOG_UNKNOWN));

  base::Pickle pickle;
  response_info_.Persist(&pickle, false, false);
  bool truncated = false;
  net::HttpResponseInfo restored_response_info;
  EXPECT_TRUE(restored_response_info.InitFromPickle(pickle, &truncated));

  response_info_.ssl_info.signed_certificate_timestamps.push_back(
      SignedCertificateTimestampAndStatus(sct,
                                          static_cast<ct::SCTVerifyStatus>(2)));
  base::Pickle pickle_invalid;
  response_info_.Persist(&pickle_invalid, false, false);
  net::HttpResponseInfo restored_invalid_response;
  EXPECT_FALSE(
      restored_invalid_response.InitFromPickle(pickle_invalid, &truncated));
}

// Test that key_exchange_group is preserved for ECDHE ciphers.
TEST_F(HttpResponseInfoTest, KeyExchangeGroupECDHE) {
  response_info_.ssl_info.cert =
      ImportCertFromFile(GetTestCertsDirectory(), "ok_cert.pem");
  SSLConnectionStatusSetVersion(SSL_CONNECTION_VERSION_TLS1_2,
                                &response_info_.ssl_info.connection_status);
  SSLConnectionStatusSetCipherSuite(
      0xcca8 /* TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 */,
      &response_info_.ssl_info.connection_status);
  response_info_.ssl_info.key_exchange_group = 23;  // X25519
  net::HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_EQ(23, restored_response_info.ssl_info.key_exchange_group);
}

// Test that key_exchange_group is preserved for TLS 1.3.
TEST_F(HttpResponseInfoTest, KeyExchangeGroupTLS13) {
  response_info_.ssl_info.cert =
      ImportCertFromFile(GetTestCertsDirectory(), "ok_cert.pem");
  SSLConnectionStatusSetVersion(SSL_CONNECTION_VERSION_TLS1_3,
                                &response_info_.ssl_info.connection_status);
  SSLConnectionStatusSetCipherSuite(0x1303 /* TLS_CHACHA20_POLY1305_SHA256 */,
                                    &response_info_.ssl_info.connection_status);
  response_info_.ssl_info.key_exchange_group = 23;  // X25519
  net::HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_EQ(23, restored_response_info.ssl_info.key_exchange_group);
}

// Test that key_exchange_group is discarded for non-ECDHE ciphers prior to TLS
// 1.3, to account for the historical key_exchange_info field. See
// https://crbug.com/639421.
TEST_F(HttpResponseInfoTest, LegacyKeyExchangeInfoDHE) {
  response_info_.ssl_info.cert =
      ImportCertFromFile(GetTestCertsDirectory(), "ok_cert.pem");
  SSLConnectionStatusSetVersion(SSL_CONNECTION_VERSION_TLS1_2,
                                &response_info_.ssl_info.connection_status);
  SSLConnectionStatusSetCipherSuite(
      0x0093 /* TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 */,
      &response_info_.ssl_info.connection_status);
  response_info_.ssl_info.key_exchange_group = 1024;
  net::HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_EQ(0, restored_response_info.ssl_info.key_exchange_group);
}

// Test that key_exchange_group is discarded for unknown ciphers prior to TLS
// 1.3, to account for the historical key_exchange_info field. See
// https://crbug.com/639421.
TEST_F(HttpResponseInfoTest, LegacyKeyExchangeInfoUnknown) {
  response_info_.ssl_info.cert =
      ImportCertFromFile(GetTestCertsDirectory(), "ok_cert.pem");
  SSLConnectionStatusSetVersion(SSL_CONNECTION_VERSION_TLS1_2,
                                &response_info_.ssl_info.connection_status);
  SSLConnectionStatusSetCipherSuite(0xffff,
                                    &response_info_.ssl_info.connection_status);
  response_info_.ssl_info.key_exchange_group = 1024;
  net::HttpResponseInfo restored_response_info;
  PickleAndRestore(response_info_, &restored_response_info);
  EXPECT_EQ(0, restored_response_info.ssl_info.key_exchange_group);
}

// Tests that cache entries loaded over SSLv3 (no longer supported) are dropped.
TEST_F(HttpResponseInfoTest, FailsInitFromPickleWithSSLV3) {
  // A valid certificate is needed for ssl_info.is_valid() to be true.
  response_info_.ssl_info.cert =
      ImportCertFromFile(GetTestCertsDirectory(), "ok_cert.pem");

  // Non-SSLv3 versions should succeed.
  SSLConnectionStatusSetVersion(SSL_CONNECTION_VERSION_TLS1_2,
                                &response_info_.ssl_info.connection_status);
  base::Pickle tls12_pickle;
  response_info_.Persist(&tls12_pickle, false, false);
  bool truncated = false;
  net::HttpResponseInfo restored_tls12_response_info;
  EXPECT_TRUE(
      restored_tls12_response_info.InitFromPickle(tls12_pickle, &truncated));
  EXPECT_EQ(SSL_CONNECTION_VERSION_TLS1_2,
            SSLConnectionStatusToVersion(
                restored_tls12_response_info.ssl_info.connection_status));
  EXPECT_FALSE(truncated);

  // SSLv3 should fail.
  SSLConnectionStatusSetVersion(SSL_CONNECTION_VERSION_SSL3,
                                &response_info_.ssl_info.connection_status);
  base::Pickle ssl3_pickle;
  response_info_.Persist(&ssl3_pickle, false, false);
  net::HttpResponseInfo restored_ssl3_response_info;
  EXPECT_FALSE(
      restored_ssl3_response_info.InitFromPickle(ssl3_pickle, &truncated));
}

}  // namespace

}  // namespace net
