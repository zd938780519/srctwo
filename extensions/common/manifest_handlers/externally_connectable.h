// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_COMMON_MANIFEST_HANDLERS_EXTERNALLY_CONNECTABLE_H_
#define EXTENSIONS_COMMON_MANIFEST_HANDLERS_EXTERNALLY_CONNECTABLE_H_

#include <memory>
#include <string>
#include <vector>

#include "base/macros.h"
#include "extensions/common/extension.h"
#include "extensions/common/install_warning.h"
#include "extensions/common/manifest_handler.h"
#include "extensions/common/url_pattern_set.h"

namespace base {
class Value;
}

namespace extensions {

// Error constants used when parsing the externally_connectable manifest entry.
namespace externally_connectable_errors {
extern const char kErrorInvalid[];
extern const char kErrorInvalidMatchPattern[];
extern const char kErrorInvalidId[];
extern const char kErrorNothingSpecified[];
extern const char kErrorTopLevelDomainsNotAllowed[];
extern const char kErrorWildcardHostsNotAllowed[];
}  // namespace externally_connectable_errors

// Parses the externally_connectable manifest entry.
class ExternallyConnectableHandler : public ManifestHandler {
 public:
  ExternallyConnectableHandler();
  ~ExternallyConnectableHandler() override;

  bool Parse(Extension* extension, base::string16* error) override;

 private:
  const std::vector<std::string> Keys() const override;

  DISALLOW_COPY_AND_ASSIGN(ExternallyConnectableHandler);
};

// The parsed form of the externally_connectable manifest entry.
struct ExternallyConnectableInfo : public Extension::ManifestData {
 public:
  // Gets the ExternallyConnectableInfo for |extension|, or NULL if none was
  // specified.
  static ExternallyConnectableInfo* Get(const Extension* extension);

  // Tries to construct the info based on |value|, as it would have appeared in
  // the manifest. Sets |error| and returns an empty scoped_ptr on failure.
  static std::unique_ptr<ExternallyConnectableInfo> FromValue(
      const base::Value& value,
      bool allow_all_urls,
      std::vector<InstallWarning>* install_warnings,
      base::string16* error);

  ~ExternallyConnectableInfo() override;

  // The URL patterns that are allowed to connect/sendMessage.
  const URLPatternSet matches;

  // The extension IDs that are allowed to connect/sendMessage. Sorted.
  const std::vector<std::string> ids;

  // True if any extension is allowed to connect. This would have corresponded
  // to an ID of "*" in |ids|.
  const bool all_ids;

  // True if extension accepts the TLS channel ID, when requested by the
  // connecting origin.
  const bool accepts_tls_channel_id;

  // Returns true if |ids| contains |id| or if |all_ids| is true.
  //
  // More convenient for callers than checking each individually, and it makes
  // use of the sortedness of |ids|.
  bool IdCanConnect(const std::string& id);

  // Public only for testing. Use FromValue in production.
  ExternallyConnectableInfo(const URLPatternSet& matches,
                            const std::vector<std::string>& ids,
                            bool all_ids,
                            bool accepts_tls_channel_id);

 private:
  DISALLOW_COPY_AND_ASSIGN(ExternallyConnectableInfo);
};

}  // namespace extensions

#endif  // EXTENSIONS_COMMON_MANIFEST_HANDLERS_EXTERNALLY_CONNECTABLE_H_
