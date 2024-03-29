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

#include "modules/quota/WorkerNavigatorStorageQuota.h"

#include "modules/quota/DeprecatedStorageQuota.h"
#include "modules/quota/StorageManager.h"

namespace blink {

WorkerNavigatorStorageQuota::WorkerNavigatorStorageQuota() {}

const char* WorkerNavigatorStorageQuota::SupplementName() {
  return "WorkerNavigatorStorageQuota";
}

WorkerNavigatorStorageQuota& WorkerNavigatorStorageQuota::From(
    WorkerNavigator& navigator) {
  WorkerNavigatorStorageQuota* supplement =
      static_cast<WorkerNavigatorStorageQuota*>(
          Supplement<WorkerNavigator>::From(navigator, SupplementName()));
  if (!supplement) {
    supplement = new WorkerNavigatorStorageQuota();
    ProvideTo(navigator, SupplementName(), supplement);
  }
  return *supplement;
}

StorageManager* WorkerNavigatorStorageQuota::storage(
    WorkerNavigator& navigator) {
  return WorkerNavigatorStorageQuota::From(navigator).storage();
}

StorageManager* WorkerNavigatorStorageQuota::storage() const {
  if (!storage_manager_)
    storage_manager_ = new StorageManager();
  return storage_manager_.Get();
}

DEFINE_TRACE(WorkerNavigatorStorageQuota) {
  visitor->Trace(storage_manager_);
  Supplement<WorkerNavigator>::Trace(visitor);
}

}  // namespace blink
