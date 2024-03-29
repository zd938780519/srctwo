// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cdm/ppapi/external_clear_key/clear_key_persistent_session_cdm.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/memory/ref_counted.h"
#include "media/base/cdm_promise.h"

namespace media {

namespace {

// When creating a persistent session, we need to add the session ID to the
// set of open persistent sessions. However, since the session ID is not
// assigned until the end, this promise is needed to capture the resulting
// session ID.
class NewPersistentSessionCdmPromise : public NewSessionCdmPromise {
 public:
  using SessionCreatedCB =
      base::OnceCallback<void(const std::string& session_id)>;

  NewPersistentSessionCdmPromise(SessionCreatedCB session_created_cb,
                                 std::unique_ptr<NewSessionCdmPromise> promise)
      : session_created_cb_(std::move(session_created_cb)),
        promise_(std::move(promise)) {}
  ~NewPersistentSessionCdmPromise() override {}

  // NewSessionCdmPromise implementation.
  void resolve(const std::string& session_id) override {
    MarkPromiseSettled();
    std::move(session_created_cb_).Run(session_id);
    promise_->resolve(session_id);
  }

  void reject(CdmPromise::Exception exception_code,
              uint32_t system_code,
              const std::string& error_message) override {
    MarkPromiseSettled();
    promise_->reject(exception_code, system_code, error_message);
  }

 private:
  SessionCreatedCB session_created_cb_;
  std::unique_ptr<NewSessionCdmPromise> promise_;

  DISALLOW_COPY_AND_ASSIGN(NewPersistentSessionCdmPromise);
};

// When a session has been loaded, we need to call FinishUpdate() to complete
// the loading of the session (to resolve the promise, generate events, etc.
// in the correct order). UpdateSession() needs a SimpleCdmPromise, but
// LoadSession needs to be resolved with the session ID. This class simply
// maps the resolve/reject call to do the right thing for NewSessionCdmPromise.
class FinishLoadCdmPromise : public SimpleCdmPromise {
 public:
  FinishLoadCdmPromise(const std::string& session_id,
                       std::unique_ptr<NewSessionCdmPromise> promise)
      : session_id_(session_id), promise_(std::move(promise)) {}
  ~FinishLoadCdmPromise() override {}

  // CdmSimplePromise implementation.
  void resolve() override {
    MarkPromiseSettled();
    promise_->resolve(session_id_);
  }

  void reject(CdmPromise::Exception exception_code,
              uint32_t system_code,
              const std::string& error_message) override {
    // Return an empty string to indicate that the session was not found.
    MarkPromiseSettled();
    promise_->resolve(std::string());
  }

 private:
  std::string session_id_;
  std::unique_ptr<NewSessionCdmPromise> promise_;

  DISALLOW_COPY_AND_ASSIGN(FinishLoadCdmPromise);
};

}  // namespace

ClearKeyPersistentSessionCdm::ClearKeyPersistentSessionCdm(
    ClearKeyCdmHost* host,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb)
    : host_(host), session_closed_cb_(session_closed_cb), weak_factory_(this) {
  cdm_ = base::MakeRefCounted<AesDecryptor>(
      session_message_cb,
      base::Bind(&ClearKeyPersistentSessionCdm::OnSessionClosed,
                 weak_factory_.GetWeakPtr()),
      session_keys_change_cb, session_expiration_update_cb);
}

void ClearKeyPersistentSessionCdm::SetServerCertificate(
    const std::vector<uint8_t>& certificate,
    std::unique_ptr<SimpleCdmPromise> promise) {
  cdm_->SetServerCertificate(certificate, std::move(promise));
}

void ClearKeyPersistentSessionCdm::CreateSessionAndGenerateRequest(
    CdmSessionType session_type,
    EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    std::unique_ptr<NewSessionCdmPromise> promise) {
  std::unique_ptr<NewSessionCdmPromise> new_promise;
  if (session_type != CdmSessionType::PERSISTENT_LICENSE_SESSION) {
    new_promise = std::move(promise);
  } else {
    // Since it's a persistent session, we need to save the session ID after
    // it's been created.
    new_promise = base::MakeUnique<NewPersistentSessionCdmPromise>(
        base::Bind(&ClearKeyPersistentSessionCdm::AddPersistentSession,
                   weak_factory_.GetWeakPtr()),
        std::move(promise));
  }
  cdm_->CreateSessionAndGenerateRequest(session_type, init_data_type, init_data,
                                        std::move(new_promise));
}

void ClearKeyPersistentSessionCdm::LoadSession(
    CdmSessionType session_type,
    const std::string& session_id,
    std::unique_ptr<NewSessionCdmPromise> promise) {
  DCHECK_EQ(CdmSessionType::PERSISTENT_LICENSE_SESSION, session_type);

  // Load the saved state for |session_id| and then create the session.
  std::unique_ptr<CdmFileAdapter> file(new CdmFileAdapter(host_));
  CdmFileAdapter* file_ref = file.get();
  file_ref->Open(
      session_id,
      base::Bind(&ClearKeyPersistentSessionCdm::OnFileOpenedForLoadSession,
                 weak_factory_.GetWeakPtr(), session_id, base::Passed(&file),
                 base::Passed(std::move(promise))));
}

void ClearKeyPersistentSessionCdm::OnFileOpenedForLoadSession(
    const std::string& session_id,
    std::unique_ptr<CdmFileAdapter> file,
    std::unique_ptr<NewSessionCdmPromise> promise,
    CdmFileAdapter::Status status) {
  if (status != CdmFileAdapter::Status::kSuccess) {
    // If unable to get data for the session, we can't load it. Return an empty
    // string to indicate that the session was not found.
    promise->resolve(std::string());
    return;
  }

  CdmFileAdapter* file_reader = file.get();
  file_reader->Read(base::Bind(
      &ClearKeyPersistentSessionCdm::OnFileReadForLoadSession,
      weak_factory_.GetWeakPtr(), session_id, base::Passed(std::move(file)),
      base::Passed(std::move(promise))));
}

void ClearKeyPersistentSessionCdm::OnFileReadForLoadSession(
    const std::string& session_id,
    std::unique_ptr<CdmFileAdapter> file,
    std::unique_ptr<NewSessionCdmPromise> promise,
    bool success,
    const std::vector<uint8_t>& data) {
  if (!success) {
    // If unable to get data for the session, so most likely the file doesn't
    // exist. Return an empty string to indicate that the session was not found.
    promise->resolve(std::string());
    return;
  }

  // Add the session to the list of active sessions.
  if (!cdm_->CreateSession(session_id,
                           CdmSessionType::PERSISTENT_LICENSE_SESSION)) {
    // If the session can't be created it's due to an already existing session
    // with the same name.
    promise->reject(CdmPromise::Exception::QUOTA_EXCEEDED_ERROR, 0,
                    "Session already exists.");
    return;
  }
  AddPersistentSession(session_id);

  // Set the session's state using the data just read.
  bool key_added = false;
  CdmPromise::Exception exception;
  std::string error_message;
  if (!cdm_->UpdateSessionWithJWK(session_id,
                                  std::string(data.begin(), data.end()),
                                  &key_added, &exception, &error_message)) {
    NOTREACHED() << "Saved session data is not usable, error = "
                 << error_message;
    // Return an empty string to indicate that the session was not found.
    promise->resolve(std::string());
    return;
  }

  // FinishUpdate() needs a SimpleCdmPromise, so create a wrapper promise.
  std::unique_ptr<SimpleCdmPromise> simple_promise(
      new FinishLoadCdmPromise(session_id, std::move(promise)));
  cdm_->FinishUpdate(session_id, key_added, std::move(simple_promise));
}

void ClearKeyPersistentSessionCdm::UpdateSession(
    const std::string& session_id,
    const std::vector<uint8_t>& response,
    std::unique_ptr<SimpleCdmPromise> promise) {
  CHECK(!response.empty());

  auto it = persistent_sessions_.find(session_id);
  if (it == persistent_sessions_.end()) {
    // Not a persistent session, so simply pass the request on.
    cdm_->UpdateSession(session_id, response, std::move(promise));
    return;
  }

  bool key_added = false;
  CdmPromise::Exception exception;
  std::string error_message;
  if (!cdm_->UpdateSessionWithJWK(session_id,
                                  std::string(response.begin(), response.end()),
                                  &key_added, &exception, &error_message)) {
    promise->reject(exception, 0, error_message);
    return;
  }

  // Persistent session has been updated, so save the current state.
  std::unique_ptr<CdmFileAdapter> file(new CdmFileAdapter(host_));
  CdmFileAdapter* file_ref = file.get();
  file_ref->Open(
      session_id,
      base::Bind(&ClearKeyPersistentSessionCdm::OnFileOpenedForUpdateSession,
                 weak_factory_.GetWeakPtr(), session_id, key_added,
                 base::Passed(&file), base::Passed(std::move(promise))));
}

void ClearKeyPersistentSessionCdm::OnFileOpenedForUpdateSession(
    const std::string& session_id,
    bool key_added,
    std::unique_ptr<CdmFileAdapter> file,
    std::unique_ptr<SimpleCdmPromise> promise,
    CdmFileAdapter::Status status) {
  if (status != CdmFileAdapter::Status::kSuccess) {
    // Unable to open the file, so the state can't be saved.
    promise->reject(CdmPromise::Exception::INVALID_STATE_ERROR, 0,
                    "Unable to save session state.");
    return;
  }

  // Grab the current session state and save it.
  std::string current_state = cdm_->GetSessionStateAsJWK(session_id);
  CdmFileAdapter* file_writer = file.get();
  file_writer->Write(
      std::vector<uint8_t>(current_state.begin(), current_state.end()),
      base::Bind(&ClearKeyPersistentSessionCdm::OnFileWrittenForUpdateSession,
                 weak_factory_.GetWeakPtr(), session_id, key_added,
                 base::Passed(std::move(file)),
                 base::Passed(std::move(promise))));
}

void ClearKeyPersistentSessionCdm::OnFileWrittenForUpdateSession(
    const std::string& session_id,
    bool key_added,
    std::unique_ptr<CdmFileAdapter> file,
    std::unique_ptr<SimpleCdmPromise> promise,
    bool success) {
  if (!success) {
    // Unable to save the state.
    promise->reject(CdmPromise::Exception::INVALID_STATE_ERROR, 0,
                    "Unable to save session state.");
    return;
  }

  cdm_->FinishUpdate(session_id, key_added, std::move(promise));
}

void ClearKeyPersistentSessionCdm::CloseSession(
    const std::string& session_id,
    std::unique_ptr<SimpleCdmPromise> promise) {
  cdm_->CloseSession(session_id, std::move(promise));
}

void ClearKeyPersistentSessionCdm::RemoveSession(
    const std::string& session_id,
    std::unique_ptr<SimpleCdmPromise> promise) {
  auto it = persistent_sessions_.find(session_id);
  if (it == persistent_sessions_.end()) {
    // Not a persistent session, so simply pass the request on.
    cdm_->RemoveSession(session_id, std::move(promise));
    return;
  }

  // Remove the saved state for |session_id| first.
  std::unique_ptr<CdmFileAdapter> file(new CdmFileAdapter(host_));
  CdmFileAdapter* file_ref = file.get();
  file_ref->Open(
      session_id,
      base::Bind(&ClearKeyPersistentSessionCdm::OnFileOpenedForRemoveSession,
                 weak_factory_.GetWeakPtr(), session_id, base::Passed(&file),
                 base::Passed(std::move(promise))));
}

void ClearKeyPersistentSessionCdm::OnFileOpenedForRemoveSession(
    const std::string& session_id,
    std::unique_ptr<CdmFileAdapter> file,
    std::unique_ptr<SimpleCdmPromise> promise,
    CdmFileAdapter::Status status) {
  if (status != CdmFileAdapter::Status::kSuccess) {
    // If no saved data, simply call RemoveSession().
    cdm_->RemoveSession(session_id, std::move(promise));
    return;
  }

  // Write out 0 length data to erase the file.
  CdmFileAdapter* file_writer = file.get();
  file_writer->Write(
      std::vector<uint8_t>(),
      base::Bind(&ClearKeyPersistentSessionCdm::OnFileWrittenForRemoveSession,
                 weak_factory_.GetWeakPtr(), session_id,
                 base::Passed(std::move(file)),
                 base::Passed(std::move(promise))));
}

void ClearKeyPersistentSessionCdm::OnFileWrittenForRemoveSession(
    const std::string& session_id,
    std::unique_ptr<CdmFileAdapter> file,
    std::unique_ptr<SimpleCdmPromise> promise,
    bool success) {
  DCHECK(success);
  cdm_->RemoveSession(session_id, std::move(promise));
}

CdmContext* ClearKeyPersistentSessionCdm::GetCdmContext() {
  return cdm_.get();
}

void ClearKeyPersistentSessionCdm::AddPersistentSession(
    const std::string& session_id) {
  persistent_sessions_.insert(session_id);
}

void ClearKeyPersistentSessionCdm::OnSessionClosed(
    const std::string& session_id) {
  persistent_sessions_.erase(session_id);
  session_closed_cb_.Run(session_id);
}

}  // namespace media
