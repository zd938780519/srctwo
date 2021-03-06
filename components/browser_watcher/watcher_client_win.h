// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_BROWSER_WATCHER_WATCHER_CLIENT_WIN_H_
#define COMPONENTS_BROWSER_WATCHER_WATCHER_CLIENT_WIN_H_

#include "base/callback.h"
#include "base/macros.h"
#include "base/process/process.h"

namespace base {
class CommandLine;
}  // namespace base

namespace browser_watcher {

// An interface class to take care of the details in launching a browser
// watch process.
class WatcherClient {
 public:
  // A CommandLineGenerator generates command lines that will launch a separate
  // process and pass the supplied HANDLE to ExitCodeWatcher in that process.
  typedef base::Callback<base::CommandLine(HANDLE)> CommandLineGenerator;

  // Constructs a watcher client that launches its watcher process using the
  // command line generated by |command_line_generator|.
  explicit WatcherClient(const CommandLineGenerator& command_line_generator);

  ~WatcherClient();

  // Launches the watcher process such that the child process is able to inherit
  // a handle to the current process.
  void LaunchWatcher();

  // Ensures that |handle| may be inherited by the watcher process. |handle|
  // must still be inheritable, and it's the client's responsibility to
  // communicate the value of |handle| to the launched process.
  void AddInheritedHandle(HANDLE handle);

  // Returns the launched process.
  const base::Process& process() const { return process_; }

 private:
  // The CommandLineGenerator passed to the constructor.
  CommandLineGenerator command_line_generator_;

  // A handle to the launched watcher process. Valid after a successful
  // LaunchWatcher() call.
  base::Process process_;

  std::vector<HANDLE> inherited_handles_;

  DISALLOW_COPY_AND_ASSIGN(WatcherClient);
};

}  // namespace browser_watcher

#endif  // COMPONENTS_BROWSER_WATCHER_WATCHER_CLIENT_WIN_H_
