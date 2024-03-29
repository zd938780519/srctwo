// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/platform/InterfaceRegistry.h"

#include "platform/wtf/StdLibExtras.h"

namespace blink {
namespace {

class EmptyInterfaceRegistry : public InterfaceRegistry {
  void AddInterface(const char* name,
                    const InterfaceFactory& factory) override {}
};

}  // namespace

InterfaceRegistry* InterfaceRegistry::GetEmptyInterfaceRegistry() {
  DEFINE_STATIC_LOCAL(EmptyInterfaceRegistry, empty_interface_registry, ());
  return &empty_interface_registry;
}

}  // namespace blink
