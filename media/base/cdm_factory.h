// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CDM_FACTORY_H_
#define MEDIA_BASE_CDM_FACTORY_H_

#include <string>

#include "base/macros.h"
#include "media/base/content_decryption_module.h"
#include "media/base/media_export.h"

namespace url {
class Origin;
}

namespace media {

// Callback used when CDM is created. |error_message| only used if
// ContentDecryptionModule is null (i.e. CDM can't be created).
using CdmCreatedCB =
    base::Callback<void(const scoped_refptr<ContentDecryptionModule>&,
                        const std::string& error_message)>;

struct CdmConfig;

class MEDIA_EXPORT CdmFactory {
 public:
  CdmFactory();
  virtual ~CdmFactory();

  // Creates a CDM for |key_system| and returns it through |cdm_created_cb|
  // asynchronously.
  virtual void Create(
      const std::string& key_system,
      const url::Origin& security_origin,
      const CdmConfig& cdm_config,
      const SessionMessageCB& session_message_cb,
      const SessionClosedCB& session_closed_cb,
      const SessionKeysChangeCB& session_keys_change_cb,
      const SessionExpirationUpdateCB& session_expiration_update_cb,
      const CdmCreatedCB& cdm_created_cb) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(CdmFactory);
};

}  // namespace media

#endif  // MEDIA_BASE_CDM_FACTORY_H_
