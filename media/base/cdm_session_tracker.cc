// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/cdm_session_tracker.h"

namespace media {

CdmSessionTracker::CdmSessionTracker() {}

CdmSessionTracker::~CdmSessionTracker() {
  DCHECK(!HasRemainingSessions());
}

void CdmSessionTracker::AddSession(const std::string& session_id) {
  DCHECK(session_ids_.find(session_id) == session_ids_.end());
  session_ids_.insert(session_id);
}

void CdmSessionTracker::RemoveSession(const std::string& session_id) {
  auto it = session_ids_.find(session_id);
  DCHECK(it != session_ids_.end());
  session_ids_.erase(it);
}

void CdmSessionTracker::CloseRemainingSessions(
    const SessionClosedCB& session_closed_cb) {
  std::unordered_set<std::string> session_ids;
  session_ids.swap(session_ids_);

  for (const auto& session_id : session_ids)
    session_closed_cb.Run(session_id);
}

bool CdmSessionTracker::HasRemainingSessions() const {
  return !session_ids_.empty();
}

}  // namespace media
