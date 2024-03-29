// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_SSL_CHROME_EXPECT_CT_REPORTER_H_
#define CHROME_BROWSER_SSL_CHROME_EXPECT_CT_REPORTER_H_

#include <map>
#include <memory>

#include "base/macros.h"
#include "net/http/transport_security_state.h"
#include "net/url_request/url_request.h"

namespace net {
class ReportSender;
class URLRequestContext;
}  // namespace net

// This class monitors for violations of CT policy and sends reports
// about failures for sites that have opted in. Must be deleted before
// the URLRequestContext that is passed to the constructor, so that it
// can cancel its requests.
//
// Since reports are sent with a non-CORS-whitelisted Content-Type, this class
// sends CORS preflight requests before sending reports. Expect-CT is not
// evaluated with a particular frame or request as context, so the preflight
// request contains an `Origin: null` header instead of a particular origin.
class ChromeExpectCTReporter
    : public net::TransportSecurityState::ExpectCTReporter,
      net::URLRequest::Delegate {
 public:
  explicit ChromeExpectCTReporter(net::URLRequestContext* request_context);
  ~ChromeExpectCTReporter() override;

  // net::ExpectCTReporter:
  void OnExpectCTFailed(const net::HostPortPair& host_port_pair,
                        const GURL& report_uri,
                        base::Time expiration,
                        const net::X509Certificate* validated_certificate_chain,
                        const net::X509Certificate* served_certificate_chain,
                        const net::SignedCertificateTimestampAndStatusList&
                            signed_certificate_timestamps) override;

  // net::URLRequest::Delegate:
  void OnResponseStarted(net::URLRequest* request, int net_error) override;
  void OnReadCompleted(net::URLRequest* request, int bytes_read) override;

 private:
  // Used to keep track of in-flight CORS preflight requests. When |request|
  // completes successfully and the CORS check passes, |serialized_report| will
  // be sent to |report_uri| using |report_sender_|.
  struct PreflightInProgress {
    PreflightInProgress(std::unique_ptr<net::URLRequest> request,
                        const std::string& serialized_report,
                        const GURL& report_uri);
    ~PreflightInProgress();
    // The preflight request.
    const std::unique_ptr<net::URLRequest> request;
    // |serialized_report| should be sent to |report_uri| if the preflight
    // succeeds.
    const std::string serialized_report;
    const GURL report_uri;
  };

  FRIEND_TEST_ALL_PREFIXES(ChromeExpectCTReporterTest, FeatureDisabled);
  FRIEND_TEST_ALL_PREFIXES(ChromeExpectCTReporterTest, EmptyReportURI);
  FRIEND_TEST_ALL_PREFIXES(ChromeExpectCTReporterTest, SendReport);
  FRIEND_TEST_ALL_PREFIXES(ChromeExpectCTReporterTest,
                           BadCORSPreflightResponseOrigin);
  FRIEND_TEST_ALL_PREFIXES(ChromeExpectCTReporterTest,
                           BadCORSPreflightResponseMethods);
  FRIEND_TEST_ALL_PREFIXES(ChromeExpectCTReporterTest,
                           BadCORSPreflightResponseHeaders);

  // Starts a CORS preflight request to obtain permission from the server to
  // send a report with Content-Type: application/expect-ct-report+json. The
  // preflight result is checked in OnResponseStarted(), and an actual report is
  // sent with |report_sender_| if the preflight succeeds.
  void SendPreflight(const GURL& report_uri,
                     const std::string& serialized_report);

  std::unique_ptr<net::ReportSender> report_sender_;

  net::URLRequestContext* request_context_;

  // The CORS preflight requests, with corresponding report information, that
  // are currently in-flight. Entries in this map are deleted when the
  // preflight's OnResponseStarted() is called.
  std::map<net::URLRequest*, std::unique_ptr<PreflightInProgress>>
      inflight_preflights_;

  DISALLOW_COPY_AND_ASSIGN(ChromeExpectCTReporter);
};

#endif  // CHROME_BROWSER_SSL_CHROME_EXPECT_CT_REPORTER_H_
