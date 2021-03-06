// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_BASE_CHROMOTING_EVENT_LOG_WRITER_H_
#define REMOTING_BASE_CHROMOTING_EVENT_LOG_WRITER_H_

namespace remoting {

class ChromotingEvent;

class ChromotingEventLogWriter {
 public:
  virtual ~ChromotingEventLogWriter(){};

  virtual void SetAuthToken(const std::string& auth_token) = 0;
  virtual void SetAuthClosure(const base::Closure& closure) = 0;
  virtual void Log(const ChromotingEvent& entry) = 0;
};

}  // namespace remoting

#endif  // REMOTING_BASE_CHROMOTING_EVENT_LOG_WRITER_H_
