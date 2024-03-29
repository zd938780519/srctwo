// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/encryptedmedia/MediaKeySystemAccess.h"

#include <memory>
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "modules/encryptedmedia/ContentDecryptionModuleResultPromise.h"
#include "modules/encryptedmedia/EncryptedMediaUtils.h"
#include "modules/encryptedmedia/MediaKeySession.h"
#include "modules/encryptedmedia/MediaKeys.h"
#include "modules/encryptedmedia/MediaKeysController.h"
#include "platform/Timer.h"
#include "platform/bindings/ScriptState.h"
#include "platform/wtf/PtrUtil.h"
#include "public/platform/WebContentDecryptionModule.h"
#include "public/platform/WebEncryptedMediaTypes.h"
#include "public/platform/WebMediaKeySystemConfiguration.h"

namespace blink {

namespace {

// This class wraps the promise resolver used when creating MediaKeys
// and is passed to Chromium to fullfill the promise. This implementation of
// completeWithCdm() will resolve the promise with a new MediaKeys object,
// while completeWithError() will reject the promise with an exception.
// All other complete methods are not expected to be called, and will
// reject the promise.
class NewCdmResultPromise : public ContentDecryptionModuleResultPromise {
  WTF_MAKE_NONCOPYABLE(NewCdmResultPromise);

 public:
  NewCdmResultPromise(
      ScriptState* script_state,
      const WebVector<WebEncryptedMediaSessionType>& supported_session_types)
      : ContentDecryptionModuleResultPromise(script_state),
        supported_session_types_(supported_session_types) {}

  ~NewCdmResultPromise() override {}

  // ContentDecryptionModuleResult implementation.
  void CompleteWithContentDecryptionModule(
      WebContentDecryptionModule* cdm) override {
    // NOTE: Continued from step 2.8 of createMediaKeys().

    if (!IsValidToFulfillPromise())
      return;

    // 2.9. Let media keys be a new MediaKeys object.
    MediaKeys* media_keys = MediaKeys::Create(
        GetExecutionContext(), supported_session_types_, WTF::WrapUnique(cdm));

    // 2.10. Resolve promise with media keys.
    Resolve(media_keys);
  }

 private:
  WebVector<WebEncryptedMediaSessionType> supported_session_types_;
};

// These methods are the inverses of those with the same names in
// NavigatorRequestMediaKeySystemAccess.
static Vector<String> ConvertInitDataTypes(
    const WebVector<WebEncryptedMediaInitDataType>& init_data_types) {
  Vector<String> result(init_data_types.size());
  for (size_t i = 0; i < init_data_types.size(); i++)
    result[i] =
        EncryptedMediaUtils::ConvertFromInitDataType(init_data_types[i]);
  return result;
}

static HeapVector<MediaKeySystemMediaCapability> ConvertCapabilities(
    const WebVector<WebMediaKeySystemMediaCapability>& capabilities) {
  HeapVector<MediaKeySystemMediaCapability> result(capabilities.size());
  for (size_t i = 0; i < capabilities.size(); i++) {
    MediaKeySystemMediaCapability capability;
    capability.setContentType(capabilities[i].content_type);
    capability.setRobustness(capabilities[i].robustness);
    result[i] = capability;
  }
  return result;
}

static String ConvertMediaKeysRequirement(
    WebMediaKeySystemConfiguration::Requirement requirement) {
  switch (requirement) {
    case WebMediaKeySystemConfiguration::Requirement::kRequired:
      return "required";
    case WebMediaKeySystemConfiguration::Requirement::kOptional:
      return "optional";
    case WebMediaKeySystemConfiguration::Requirement::kNotAllowed:
      return "not-allowed";
  }

  NOTREACHED();
  return "not-allowed";
}

static Vector<String> ConvertSessionTypes(
    const WebVector<WebEncryptedMediaSessionType>& session_types) {
  Vector<String> result(session_types.size());
  for (size_t i = 0; i < session_types.size(); i++)
    result[i] = EncryptedMediaUtils::ConvertFromSessionType(session_types[i]);
  return result;
}

}  // namespace

MediaKeySystemAccess::MediaKeySystemAccess(
    const String& key_system,
    std::unique_ptr<WebContentDecryptionModuleAccess> access)
    : key_system_(key_system), access_(std::move(access)) {}

MediaKeySystemAccess::~MediaKeySystemAccess() {}

void MediaKeySystemAccess::getConfiguration(
    MediaKeySystemConfiguration& result) {
  WebMediaKeySystemConfiguration configuration = access_->GetConfiguration();

  // |initDataTypes|, |audioCapabilities|, and |videoCapabilities| can only be
  // empty if they were not present in the requested configuration.
  if (!configuration.init_data_types.IsEmpty())
    result.setInitDataTypes(
        ConvertInitDataTypes(configuration.init_data_types));
  if (!configuration.audio_capabilities.IsEmpty())
    result.setAudioCapabilities(
        ConvertCapabilities(configuration.audio_capabilities));
  if (!configuration.video_capabilities.IsEmpty())
    result.setVideoCapabilities(
        ConvertCapabilities(configuration.video_capabilities));

  // |distinctiveIdentifier|, |persistentState|, and |sessionTypes| are always
  // set by requestMediaKeySystemAccess().
  result.setDistinctiveIdentifier(
      ConvertMediaKeysRequirement(configuration.distinctive_identifier));
  result.setPersistentState(
      ConvertMediaKeysRequirement(configuration.persistent_state));
  result.setSessionTypes(ConvertSessionTypes(configuration.session_types));

  // |label| will (and should) be a null string if it was not set.
  result.setLabel(configuration.label);
}

ScriptPromise MediaKeySystemAccess::createMediaKeys(ScriptState* script_state) {
  // From http://w3c.github.io/encrypted-media/#createMediaKeys
  // (Reordered to be able to pass values into the promise constructor.)
  // 2.4 Let configuration be the value of this object's configuration value.
  // 2.5-2.8. [Set use distinctive identifier and persistent state allowed
  //          based on configuration.]
  WebMediaKeySystemConfiguration configuration = access_->GetConfiguration();

  // 1. Let promise be a new promise.
  NewCdmResultPromise* helper =
      new NewCdmResultPromise(script_state, configuration.session_types);
  ScriptPromise promise = helper->Promise();

  // 2. Asynchronously create and initialize the MediaKeys object.
  // 2.1 Let cdm be the CDM corresponding to this object.
  // 2.2 Load and initialize the cdm if necessary.
  // 2.3 If cdm fails to load or initialize, reject promise with a new
  //     DOMException whose name is the appropriate error name.
  //     (Done if completeWithException() called).
  access_->CreateContentDecryptionModule(helper->Result());

  // 3. Return promise.
  return promise;
}

DEFINE_TRACE(MediaKeySystemAccess) {}

}  // namespace blink
