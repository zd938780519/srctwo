// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_PROXY_PROXY_RESOLVER_FACTORY_H_
#define NET_PROXY_PROXY_RESOLVER_FACTORY_H_

#include <memory>
#include <set>

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "net/base/completion_callback.h"
#include "net/base/net_export.h"
#include "net/proxy/proxy_resolver_script_data.h"

namespace net {

class ProxyResolver;

// ProxyResolverFactory is an interface for creating ProxyResolver instances.
class NET_EXPORT ProxyResolverFactory {
 public:
  // A handle to a request. Deleting it will cancel the request.
  class Request {
   public:
    virtual ~Request() {}
  };

  // See |expects_pac_bytes()| for the meaning of |expects_pac_bytes|.
  explicit ProxyResolverFactory(bool expects_pac_bytes);

  virtual ~ProxyResolverFactory();

  // Creates a new ProxyResolver. If the request will complete asynchronously,
  // it returns ERR_IO_PENDING and notifies the result by running |callback|.
  // If the result is OK, then |resolver| contains the ProxyResolver. In the
  // case of asynchronous completion |*request| is written to, and can be
  // deleted to cancel the request. All requests in progress are cancelled if
  // the ProxyResolverFactory is deleted.
  virtual int CreateProxyResolver(
      const scoped_refptr<ProxyResolverScriptData>& pac_script,
      std::unique_ptr<ProxyResolver>* resolver,
      const net::CompletionCallback& callback,
      std::unique_ptr<Request>* request) = 0;

  // The PAC script backend can be specified to the ProxyResolverFactory either
  // via URL, or via the javascript text itself. If |expects_pac_bytes| is true,
  // then the ProxyResolverScriptData passed to CreateProxyResolver() should
  // contain the actual script bytes rather than just the URL.
  bool expects_pac_bytes() const { return expects_pac_bytes_; }

 private:
  bool expects_pac_bytes_;

  DISALLOW_COPY_AND_ASSIGN(ProxyResolverFactory);
};

}  // namespace net

#endif  // NET_PROXY_PROXY_RESOLVER_FACTORY_H_
