// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_MAILBOX_MANAGER_H_
#define GPU_COMMAND_BUFFER_SERVICE_MAILBOX_MANAGER_H_

#include <memory>

#include "base/macros.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/gpu_export.h"

namespace gpu {

struct GpuPreferences;
struct SyncToken;

namespace gles2 {

class TextureBase;

// Manages resources scoped beyond the context or context group level.
class GPU_EXPORT MailboxManager {
 public:
  virtual ~MailboxManager() {}

  // Look up the texture definition from the named mailbox.
  virtual TextureBase* ConsumeTexture(const Mailbox& mailbox) = 0;

  // Put the texture into the named mailbox.
  virtual void ProduceTexture(const Mailbox& mailbox, TextureBase* texture) = 0;

  // If |true| then Pull/PushTextureUpdates() needs to be called.
  virtual bool UsesSync() = 0;

  // Used to synchronize texture state across share groups.
  virtual void PushTextureUpdates(const SyncToken& token) = 0;
  virtual void PullTextureUpdates(const SyncToken& token) = 0;

  // Destroy any mailbox that reference the given texture.
  virtual void TextureDeleted(TextureBase* texture) = 0;

  static std::unique_ptr<MailboxManager> Create(
      const GpuPreferences& gpu_preferences);
};

}  // namespage gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_MAILBOX_MANAGER_H_

