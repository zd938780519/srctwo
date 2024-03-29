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

#include "core/timing/WorkerGlobalScopePerformance.h"

#include "core/timing/WorkerPerformance.h"
#include "core/workers/WorkerGlobalScope.h"

namespace blink {

WorkerGlobalScopePerformance::WorkerGlobalScopePerformance(
    WorkerGlobalScope& worker_global_scope)
    : Supplement<WorkerGlobalScope>(worker_global_scope) {}

const char* WorkerGlobalScopePerformance::SupplementName() {
  return "WorkerGlobalScopePerformance";
}

WorkerGlobalScopePerformance& WorkerGlobalScopePerformance::From(
    WorkerGlobalScope& worker_global_scope) {
  WorkerGlobalScopePerformance* supplement =
      static_cast<WorkerGlobalScopePerformance*>(
          Supplement<WorkerGlobalScope>::From(worker_global_scope,
                                              SupplementName()));
  if (!supplement) {
    supplement = new WorkerGlobalScopePerformance(worker_global_scope);
    ProvideTo(worker_global_scope, SupplementName(), supplement);
  }
  return *supplement;
}

WorkerPerformance* WorkerGlobalScopePerformance::performance(
    WorkerGlobalScope& worker_global_scope) {
  return From(worker_global_scope).performance(&worker_global_scope);
}

WorkerPerformance* WorkerGlobalScopePerformance::performance(
    WorkerGlobalScope* worker_global_scope) {
  if (!performance_)
    performance_ = WorkerPerformance::Create(worker_global_scope);
  return performance_.Get();
}

DEFINE_TRACE(WorkerGlobalScopePerformance) {
  visitor->Trace(performance_);
  Supplement<WorkerGlobalScope>::Trace(visitor);
}

}  // namespace blink
