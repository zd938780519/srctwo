// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_PROTOCOL_SESSION_PLUGIN_H_
#define REMOTING_PROTOCOL_SESSION_PLUGIN_H_

#include <memory>

#include "third_party/libjingle_xmpp/xmllite/xmlelement.h"

namespace remoting {
namespace protocol {

// Interface for Session plugins. Plugins allow to send and receive optional
// information that is not essential for session handshake. Messages generated
// by the plugins on one end of a connection are attached to the session
// handshake messages and passed to the plugins on the other end. Plugins are
// optional, i.e. Session doesn't need any plugins to connect successfully.
class SessionPlugin {
 public:
  SessionPlugin() = default;
  virtual ~SessionPlugin() = default;

  // Returns an XmlElement if the SessionPlugin requires to attach some data
  // into the outgoing message.
  virtual std::unique_ptr<buzz::XmlElement> GetNextMessage() = 0;

  // Handles messages in |attachments|.
  virtual void OnIncomingMessage(const buzz::XmlElement& attachments) = 0;
};

}  // namespace protocol
}  // namespace remoting

#endif  // REMOTING_PROTOCOL_SESSION_PLUGIN_H_
