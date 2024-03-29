// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_RENDERER_GC_CALLBACK_H_
#define EXTENSIONS_RENDERER_GC_CALLBACK_H_

#include <map>
#include <string>

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "v8/include/v8.h"

namespace extensions {

class ScriptContext;

// Runs |callback| when v8 garbage collects |object|, or |fallback| if
// |context| is invalidated first. Exactly one of |callback| or |fallback| will
// be called, after which it deletes itself.
class GCCallback {
 public:
  GCCallback(ScriptContext* context,
             const v8::Local<v8::Object>& object,
             const v8::Local<v8::Function>& callback,
             const base::Closure& fallback);
  ~GCCallback();

 private:
  static void OnObjectGC(const v8::WeakCallbackInfo<GCCallback>& data);
  void RunCallback();
  void OnContextInvalidated();

  // The context which owns |object_|.
  ScriptContext* context_;

  // The object this GCCallback is bound to.
  v8::Global<v8::Object> object_;

  // The function to run when |object_| is garbage collected.
  v8::Global<v8::Function> callback_;

  // The function to run if |context_| is invalidated before we have a chance
  // to execute |callback_|.
  base::Closure fallback_;

  base::WeakPtrFactory<GCCallback> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(GCCallback);
};

}  // namespace extensions

#endif  // EXTENSIONS_RENDERER_GC_CALLBACK_H_
