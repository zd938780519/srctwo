// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_COMMON_CHILD_PROCESS_HOST_DELEGATE_H_
#define CONTENT_PUBLIC_COMMON_CHILD_PROCESS_HOST_DELEGATE_H_

#include <string>

#include "base/process/process.h"
#include "content/common/content_export.h"
#include "ipc/ipc_listener.h"
#include "mojo/public/cpp/system/message_pipe.h"

namespace IPC {
class Channel;
}

namespace content {

// Interface that all users of ChildProcessHost need to provide.
class ChildProcessHostDelegate : public IPC::Listener {
 public:
  ~ChildProcessHostDelegate() override {}

  // Delegates return true if it's ok to shut down the child process (which is
  // the default return value). The exception is if the host is in the middle of
  // sending a request to the process, in which case the other side might think
  // it's ok to shutdown, when really it's not.
  CONTENT_EXPORT virtual bool CanShutdown();

  // Called when the IPC channel for the child process is initialized.
  virtual void OnChannelInitialized(IPC::Channel* channel) {}

  // Called when the child process unexpected closes the IPC channel. Delegates
  // would normally delete the object in this case.
  virtual void OnChildDisconnected() {}

  // Returns a reference to the child process. This can be called only after
  // OnProcessLaunched is called or it will be invalid and may crash.
  virtual const base::Process& GetProcess() const = 0;

  // Binds an interface in the child process.
  virtual void BindInterface(const std::string& interface_name,
                             mojo::ScopedMessagePipeHandle interface_pipe) {}
};

};  // namespace content

#endif  // CONTENT_PUBLIC_COMMON_CHILD_PROCESS_HOST_DELEGATE_H_
