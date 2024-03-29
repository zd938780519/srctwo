// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_EFFECTIVE_CONNECTION_TYPE_HELPER_H_
#define CONTENT_RENDERER_EFFECTIVE_CONNECTION_TYPE_HELPER_H_

#include "net/nqe/effective_connection_type.h"
#include "third_party/WebKit/public/platform/WebEffectiveConnectionType.h"

namespace content {

blink::WebEffectiveConnectionType
EffectiveConnectionTypeToWebEffectiveConnectionType(
    net::EffectiveConnectionType net_type);

}  // namespace content

#endif  // CONTENT_RENDERER_EFFECTIVE_CONNECTION_TYPE_HELPER_H_
