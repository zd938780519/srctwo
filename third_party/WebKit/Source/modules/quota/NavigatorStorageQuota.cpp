/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "modules/quota/NavigatorStorageQuota.h"

#include "core/frame/Navigator.h"
#include "modules/quota/DeprecatedStorageQuota.h"
#include "modules/quota/StorageManager.h"

namespace blink {

NavigatorStorageQuota::NavigatorStorageQuota(Navigator& navigator)
    : Supplement<Navigator>(navigator) {}

const char* NavigatorStorageQuota::SupplementName() {
  return "NavigatorStorageQuota";
}

NavigatorStorageQuota& NavigatorStorageQuota::From(Navigator& navigator) {
  NavigatorStorageQuota* supplement = static_cast<NavigatorStorageQuota*>(
      Supplement<Navigator>::From(navigator, SupplementName()));
  if (!supplement) {
    supplement = new NavigatorStorageQuota(navigator);
    ProvideTo(navigator, SupplementName(), supplement);
  }
  return *supplement;
}


DeprecatedStorageQuota* NavigatorStorageQuota::webkitTemporaryStorage(
    Navigator& navigator) {
  return NavigatorStorageQuota::From(navigator).webkitTemporaryStorage();
}

DeprecatedStorageQuota* NavigatorStorageQuota::webkitPersistentStorage(
    Navigator& navigator) {
  return NavigatorStorageQuota::From(navigator).webkitPersistentStorage();
}

StorageManager* NavigatorStorageQuota::storage(Navigator& navigator) {
  return NavigatorStorageQuota::From(navigator).storage();
}

DeprecatedStorageQuota* NavigatorStorageQuota::webkitTemporaryStorage() const {
  if (!temporary_storage_)
    temporary_storage_ =
        DeprecatedStorageQuota::Create(DeprecatedStorageQuota::kTemporary);
  return temporary_storage_.Get();
}

DeprecatedStorageQuota* NavigatorStorageQuota::webkitPersistentStorage() const {
  if (!persistent_storage_)
    persistent_storage_ =
        DeprecatedStorageQuota::Create(DeprecatedStorageQuota::kPersistent);
  return persistent_storage_.Get();
}

StorageManager* NavigatorStorageQuota::storage() const {
  if (!storage_manager_)
    storage_manager_ = new StorageManager();
  return storage_manager_.Get();
}

DEFINE_TRACE(NavigatorStorageQuota) {
  visitor->Trace(temporary_storage_);
  visitor->Trace(persistent_storage_);
  visitor->Trace(storage_manager_);
  Supplement<Navigator>::Trace(visitor);
}

}  // namespace blink
