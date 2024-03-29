// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_NETWORK_NETWORK_UTIL_H_
#define CHROMEOS_NETWORK_NETWORK_UTIL_H_

// This header is introduced to make it easy to switch from chromeos_network.cc
// to Chrome's own DBus code.  crosbug.com/16557
// All calls to functions in chromeos_network.h should be made through
// functions provided by this header.

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/time/time.h"
#include "base/values.h"
#include "chromeos/chromeos_export.h"

namespace base {
class ListValue;
}

namespace chromeos {

class NetworkState;
class NetworkTypePattern;

// Struct for passing wifi access point data.
struct CHROMEOS_EXPORT WifiAccessPoint {
  WifiAccessPoint();
  WifiAccessPoint(const WifiAccessPoint& other);
  ~WifiAccessPoint();
  std::string ssid;  // The ssid of the WiFi node if available.
  std::string mac_address;  // The mac address of the WiFi node.
  base::Time timestamp;  // Timestamp when this AP was detected.
  int signal_strength;  // Radio signal strength measured in dBm.
  int signal_to_noise;  // Current signal to noise ratio measured in dB.
  int channel;  // Wifi channel number.
};

// Struct for passing cellular location data
// The age, signalStrength, and timingAdvance fields are currently unused:
// https://developers.google.com/maps/documentation/geolocation/intro#cell_tower_object
struct CHROMEOS_EXPORT CellTower {
  CellTower();
  CellTower(const CellTower& other);
  ~CellTower();
  std::string mcc;       // The mobile country code if available
  std::string mnc;       // The mobile network code if available
  std::string lac;       // The location area code if available
  std::string ci;        // The cell id if availabe
  base::Time timestamp;  // Timestamp when this location was detected.
};

// Struct for passing network scan result data.
struct CHROMEOS_EXPORT CellularScanResult {
  CellularScanResult();
  CellularScanResult(const CellularScanResult& other);
  ~CellularScanResult();
  std::string status;  // The network's availability status. (One of "unknown",
                       // "available", "current", or "forbidden")
  std::string network_id;  // 3GPP operator code ("MCCMNC").
  std::string short_name;  // Short-format name of the operator.
  std::string long_name;  // Long-format name of the operator.
  std::string technology;  // Access technology.
};

typedef std::vector<WifiAccessPoint> WifiAccessPointVector;
typedef std::vector<CellTower> CellTowerVector;

// Describes whether there is an error and whether the error came from
// the local system or from the server implementing the connect
// method.
enum NetworkMethodErrorType {
  NETWORK_METHOD_ERROR_NONE = 0,
  NETWORK_METHOD_ERROR_LOCAL = 1,
  NETWORK_METHOD_ERROR_REMOTE = 2,
};

// Callback for methods that initiate an operation and return no data.
typedef base::Callback<void(
    const std::string& path,
    NetworkMethodErrorType error,
    const std::string& error_message)> NetworkOperationCallback;

namespace network_util {

// Converts a |prefix_length| to a netmask. (for IPv4 only)
// e.g. a |prefix_length| of 24 is converted to a netmask of "255.255.255.0".
// Invalid prefix lengths will return the empty string.
CHROMEOS_EXPORT std::string PrefixLengthToNetmask(int32_t prefix_length);

// Converts a |netmask| to a prefixlen. (for IPv4 only)
// e.g. a |netmask| of 255.255.255.0 is converted to a prefixlen of 24
CHROMEOS_EXPORT int32_t NetmaskToPrefixLength(const std::string& netmask);

// Returns |shill_mac_address| in aa:bb format.
CHROMEOS_EXPORT std::string FormattedMacAddress(
    const std::string& shill_mac_address);

// Parses |list|, which contains DictionaryValues and returns a vector of
// CellularScanResult in |scan_results|. Returns false if parsing fails,
// in which case the contents of |scan_results| will be undefined.
CHROMEOS_EXPORT bool ParseCellularScanResults(
    const base::ListValue& list, std::vector<CellularScanResult>* scan_results);

// Retrieves the ONC state dictionary for |network| using GetStateProperties.
// This includes properties from the corresponding NetworkState if it exists.
// Assumed to be called from the primary user profile.
CHROMEOS_EXPORT std::unique_ptr<base::DictionaryValue>
TranslateNetworkStateToONC(const NetworkState* network);

// Retrieves the list of network services by passing |pattern|,
// |configured_only|, and |visible_only| to NetworkStateHandler::
// GetNetworkListByType(). Translates the result into a list of ONC
// dictionaries using TranslateShillServiceToONCPart. |limit| is used to limit
// the number of results.
CHROMEOS_EXPORT std::unique_ptr<base::ListValue> TranslateNetworkListToONC(
    NetworkTypePattern pattern,
    bool configured_only,
    bool visible_only,
    int limit);

// Returns the Shill type corresponding to ONC |type| or an empty string if
// there is no match. Only valid for ethernet, wifi, wimax, cellular, and vpn.
CHROMEOS_EXPORT std::string TranslateONCTypeToShill(const std::string& type);

// Inverse of TranslateONCTypeToShill.
CHROMEOS_EXPORT std::string TranslateShillTypeToONC(const std::string& type);

}  // namespace network_util
}  // namespace chromeos

#endif  // CHROMEOS_NETWORK_NETWORK_UTIL_H_
