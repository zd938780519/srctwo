// Copyright 2017 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "snapshot/linux/thread_snapshot_linux.h"

#include <sched.h>

#include "base/logging.h"
#include "snapshot/linux/cpu_context_linux.h"
#include "util/misc/reinterpret_bytes.h"

namespace crashpad {
namespace internal {

ThreadSnapshotLinux::ThreadSnapshotLinux()
    : ThreadSnapshot(),
      context_union_(),
      context_(),
      stack_(),
      thread_specific_data_address_(0),
      thread_id_(-1),
      priority_(-1),
      initialized_() {
}

ThreadSnapshotLinux::~ThreadSnapshotLinux() {
}

bool ThreadSnapshotLinux::Initialize(
    ProcessReader* process_reader,
    const ProcessReader::Thread& thread) {
  INITIALIZATION_STATE_SET_INITIALIZING(initialized_);

#if defined(ARCH_CPU_X86_FAMILY)
  if (process_reader->Is64Bit()) {
    context_.architecture = kCPUArchitectureX86_64;
    context_.x86_64 = &context_union_.x86_64;
    InitializeCPUContextX86_64(thread.thread_context.t64,
                               thread.float_context.f64,
                               context_.x86_64);
  } else {
    context_.architecture = kCPUArchitectureX86;
    context_.x86 = &context_union_.x86;
    InitializeCPUContextX86(thread.thread_context.t32,
                            thread.float_context.f32,
                            context_.x86);
  }
#else
#error Port.
#endif

  stack_.Initialize(process_reader,
                    thread.stack_region_address,
                    thread.stack_region_size);

  thread_specific_data_address_ =
      thread.thread_specific_data_address;

  thread_id_ = thread.tid;

  // Map Linux scheduling policy, static priority, and nice value into a single
  // int value.
  //
  // The possible policies in order of approximate priority (low to high) are
  //   SCHED_IDLE
  //   SCHED_BATCH
  //   SCHED_OTHER
  //   SCHED_RR
  //   SCHED_FIFO
  //
  // static_priority is not used for OTHER, BATCH, or IDLE and should be 0.
  // For FIFO and RR, static_priority should range from 1 to 99 with 99 being
  // the highest priority.
  //
  // nice value ranges from -20 to 19, with -20 being highest priority

  enum class Policy : uint8_t {
    kUnknown = 0,
    kIdle,
    kBatch,
    kOther,
    kRR,
    kFIFO
  };

  struct LinuxPriority {
#if defined(ARCH_CPU_LITTLE_ENDIAN)
    // nice values affect how dynamic priorities are updated, which only matters
    // for threads with the same static priority.
    uint8_t nice_value = 0;

    // The scheduling policy also affects how threads with the same static
    // priority are ordered, but has greater impact than nice value.
    Policy policy = Policy::kUnknown;

    // The static priority is the most significant in determining overall
    // priority.
    uint8_t static_priority = 0;

    // Put this in the most significant byte position to prevent negative
    // priorities.
    uint8_t unused = 0;
#elif defined(ARCH_CPU_BIG_ENDIAN)
    uint8_t unused = 0;
    uint8_t static_priority = 0;
    Policy policy = Policy::kUnknown;
    uint8_t nice_value = 0;
#endif  // ARCH_CPU_LITTLE_ENDIAN
  };
  static_assert(sizeof(LinuxPriority) <= sizeof(int), "priority is too large");

  LinuxPriority prio;

  // Lower nice values have higher priority, so negate them and add 20 to put
  // them in the range 1-40 with 40 being highest priority.
  if (thread.nice_value < -20 || thread.nice_value > 19) {
    LOG(WARNING) << "invalid nice value " << thread.nice_value;
    prio.nice_value = 0;
  } else {
    prio.nice_value = -1 * thread.nice_value + 20;
  }

  switch (thread.sched_policy) {
    case SCHED_IDLE:
      prio.policy = Policy::kIdle;
      break;
    case SCHED_BATCH:
      prio.policy = Policy::kBatch;
      break;
    case SCHED_OTHER:
      prio.policy = Policy::kOther;
      break;
    case SCHED_RR:
      prio.policy = Policy::kRR;
      break;
    case SCHED_FIFO:
      prio.policy = Policy::kFIFO;
      break;
    default:
      prio.policy = Policy::kUnknown;
      LOG(WARNING) << "Unknown scheduling policy " << thread.sched_policy;
  }

  if (thread.static_priority < 0 || thread.static_priority > 99) {
    LOG(WARNING) << "invalid static priority " << thread.static_priority;
  }
  prio.static_priority = thread.static_priority;

  if (!ReinterpretBytes(prio, &priority_)) {
    LOG(ERROR) << "Couldn't set priority";
    return false;
  }

  INITIALIZATION_STATE_SET_VALID(initialized_);
  return true;
}

const CPUContext* ThreadSnapshotLinux::Context() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return &context_;
}

const MemorySnapshot* ThreadSnapshotLinux::Stack() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return &stack_;
}

uint64_t ThreadSnapshotLinux::ThreadID() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return thread_id_;
}

int ThreadSnapshotLinux::SuspendCount() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return 0;
}

int ThreadSnapshotLinux::Priority() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return priority_;
}

uint64_t ThreadSnapshotLinux::ThreadSpecificDataAddress() const {
  INITIALIZATION_STATE_DCHECK_VALID(initialized_);
  return thread_specific_data_address_;
}

std::vector<const MemorySnapshot*> ThreadSnapshotLinux::ExtraMemory() const {
  return std::vector<const MemorySnapshot*>();
}

}  // namespace internal
}  // namespace crashpad
