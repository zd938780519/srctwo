// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IPC_IPC_MESSAGE_ATTACHMENT_H_
#define IPC_IPC_MESSAGE_ATTACHMENT_H_

#include "base/files/file.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/pickle.h"
#include "build/build_config.h"
#include "ipc/ipc.mojom.h"
#include "ipc/ipc_export.h"

namespace IPC {

// Auxiliary data sent with |Message|. This can be a platform file descriptor
// or a mojo |MessagePipe|. |GetType()| returns the type of the subclass.
class IPC_EXPORT MessageAttachment : public base::Pickle::Attachment {
 public:
  using Type = mojom::SerializedHandle::Type;

  virtual Type GetType() const = 0;

 protected:
  friend class base::RefCountedThreadSafe<MessageAttachment>;
  MessageAttachment();
  ~MessageAttachment() override;

  DISALLOW_COPY_AND_ASSIGN(MessageAttachment);
};

}  // namespace IPC

#endif  // IPC_IPC_MESSAGE_ATTACHMENT_H_
