// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var tests = [
  function permissionWarnings() {
    var manifest_str = "{ \"name\": \"Clipboard!\", \"version\": \"1.0\", " +
                       "\"permissions\": [\"clipboardRead\"] }";

    chrome.management.getPermissionWarningsByManifest(
        manifest_str, chrome.test.callback(function(warnings) {
      chrome.test.assertEq(1, warnings.length);
      chrome.test.assertEq("Read data you copy and paste", warnings[0]);
    }));
  },
];

chrome.test.runTests(tests);
