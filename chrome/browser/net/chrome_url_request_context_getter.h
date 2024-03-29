// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NET_CHROME_URL_REQUEST_CONTEXT_GETTER_H_
#define CHROME_BROWSER_NET_CHROME_URL_REQUEST_CONTEXT_GETTER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/single_thread_task_runner.h"
#include "chrome/browser/custom_handlers/protocol_handler_registry.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_job_factory.h"

class ChromeURLRequestContextFactory;
class Profile;
class ProfileIOData;
struct StoragePartitionDescriptor;

// A net::URLRequestContextGetter subclass used by the browser. This returns a
// subclass of net::URLRequestContext which can be used to store extra
// information about requests.
//
// Most methods are expected to be called on the UI thread, except for
// the destructor and GetURLRequestContext().
class ChromeURLRequestContextGetter : public net::URLRequestContextGetter {
 public:
  // Constructs a ChromeURLRequestContextGetter that will use |factory| to
  // create the URLRequestContext.
  explicit ChromeURLRequestContextGetter(
      ChromeURLRequestContextFactory* factory);

  // Note that GetURLRequestContext() can only be called from the IO
  // thread (it will assert otherwise).
  // GetIOTaskRunner however can be called from any thread.
  //
  // net::URLRequestContextGetter implementation.
  net::URLRequestContext* GetURLRequestContext() override;
  scoped_refptr<base::SingleThreadTaskRunner> GetNetworkTaskRunner()
      const override;

  // Create an instance for use with an 'original' (non-OTR) profile. This is
  // expected to get called on the UI thread.
  static ChromeURLRequestContextGetter* Create(
      Profile* profile,
      const ProfileIOData* profile_io_data,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);

  // Create an instance for an original profile for media. This is expected to
  // get called on UI thread. This method takes a profile and reuses the
  // 'original' net::URLRequestContext for common files.
  static ChromeURLRequestContextGetter* CreateForMedia(
      Profile* profile, const ProfileIOData* profile_io_data);

  // Create an instance for an original profile for extensions. This is expected
  // to get called on UI thread.
  static ChromeURLRequestContextGetter* CreateForExtensions(
      Profile* profile, const ProfileIOData* profile_io_data);

  // Create an instance for an original profile for an app with isolated
  // storage. This is expected to get called on UI thread.
  static ChromeURLRequestContextGetter* CreateForIsolatedApp(
      Profile* profile,
      const ProfileIOData* profile_io_data,
      const StoragePartitionDescriptor& partition_descriptor,
      std::unique_ptr<ProtocolHandlerRegistry::JobInterceptorFactory>
          protocol_handler_interceptor,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);

  // Create an instance for an original profile for media with isolated
  // storage. This is expected to get called on UI thread.
  static ChromeURLRequestContextGetter* CreateForIsolatedMedia(
      Profile* profile,
      ChromeURLRequestContextGetter* app_context,
      const ProfileIOData* profile_io_data,
      const StoragePartitionDescriptor& partition_descriptor);

  // Discard reference to URLRequestContext and inform observers of shutdown.
  // Must be called before destruction. May only be called on IO thread.
  void NotifyContextShuttingDown();

 private:
  ~ChromeURLRequestContextGetter() override;

  // Deferred logic for creating a URLRequestContext.
  // Access only from the IO thread.
  std::unique_ptr<ChromeURLRequestContextFactory> factory_;

  // NULL before initialization and after invalidation.
  // Otherwise, it is the URLRequestContext instance that
  // was lazily created by GetURLRequestContext().
  // Access only from the IO thread.
  net::URLRequestContext* url_request_context_;

  DISALLOW_COPY_AND_ASSIGN(ChromeURLRequestContextGetter);
};

#endif  // CHROME_BROWSER_NET_CHROME_URL_REQUEST_CONTEXT_GETTER_H_
