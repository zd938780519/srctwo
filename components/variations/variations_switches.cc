// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/variations/variations_switches.h"

namespace variations {
namespace switches {

// Disable field trial tests configured in fieldtrial_testing_config.json.
const char kDisableFieldTrialTestingConfig[] = "disable-field-trial-config";

// TODO(asvitkine): Consider removing or renaming this functionality.
// Enables the benchmarking extensions.
const char kEnableBenchmarking[] = "enable-benchmarking";

// Fakes the channel of the browser for purposes of Variations filtering. This
// is to be used for testing only. Possible values are "stable", "beta", "dev"
// and "canary". Note that this only applies if the browser's reported channel
// is UNKNOWN.
const char kFakeVariationsChannel[] = "fake-variations-channel";

// This option can be used to force parameters of field trials when testing
// changes locally. The argument is a param list of (key, value) pairs prefixed
// by an associated (trial, group) pair. You specify the param list for multiple
// (trial, group) pairs with a comma separator.
// Example:
//   "Trial1.Group1:k1/v1/k2/v2,Trial2.Group2:k3/v3/k4/v4"
// Trial names, groups names, parameter names, and value should all be URL
// escaped for all non-alphanumeric characters.
const char kForceFieldTrialParams[] = "force-fieldtrial-params";

// Forces additional Chrome Variation Ids that will be sent in X-Client-Data
// header, specified as a 64-bit encoded list of numeric experiment ids. Ids
// prefixed with the character "t" will be treated as Trigger Variation Ids.
const char kForceVariationIds[] = "force-variation-ids";

// Allows overriding the country used for evaluating variations. This is similar
// to the "Override Variations Country" entry on chrome://translate-internals,
// but is exposed as a command-line flag to allow testing First Run scenarios.
// Additionally, unlike chrome://translate-internals, the value isn't persisted
// across sessions.
const char kVariationsOverrideCountry[] = "variations-override-country";

// Specifies a custom URL for the server which reports variation data to the
// client. Specifying this switch enables the Variations service on
// unofficial builds. See variations_service.cc.
const char kVariationsServerURL[] = "variations-server-url";

}  // namespace switches
}  // namespace variations
