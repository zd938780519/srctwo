// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NET_SERVICE_PROVIDERS_WIN_H_
#define CHROME_BROWSER_NET_SERVICE_PROVIDERS_WIN_H_

#include <vector>

#include "base/strings/string16.h"

struct WinsockNamespaceProvider {
  base::string16 name;
  int version;
  bool active;
  int type;
};
typedef std::vector<WinsockNamespaceProvider> WinsockNamespaceProviderList;

struct WinsockLayeredServiceProvider {
  WinsockLayeredServiceProvider();
  WinsockLayeredServiceProvider(const WinsockLayeredServiceProvider& other);
  ~WinsockLayeredServiceProvider();

  base::string16 name;
  base::string16 path;
  int version;
  int chain_length;
  int socket_type;
  int socket_protocol;
};
typedef std::vector<WinsockLayeredServiceProvider>
    WinsockLayeredServiceProviderList;

// Returns all the Winsock namespace providers.
void GetWinsockNamespaceProviders(WinsockNamespaceProviderList* namespace_list);

// Returns all the Winsock layered service providers and their paths.
void GetWinsockLayeredServiceProviders(
    WinsockLayeredServiceProviderList* service_list);

#endif  // CHROME_BROWSER_NET_SERVICE_PROVIDERS_WIN_H_
