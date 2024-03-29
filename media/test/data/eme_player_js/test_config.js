// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Test configuration used by test page to configure the player app and other
// test specific configurations.
function TestConfig() {
  this.mediaFile = null;
  this.keySystem = null;
  this.mediaType = null;
  this.licenseServerURL = null;
  this.useMSE = false;
  this.runFPS = false;
  this.playTwice = false;
  this.configChangeType = CONFIG_CHANGE_TYPE.CLEAR_TO_CLEAR;
  this.policyCheck = false;
}

TestConfig.prototype.loadQueryParams = function() {
  // Load query parameters and set default values.
  var r = /([^&=]+)=?([^&]*)/g;
  // Lambda function for decoding extracted match values. Replaces '+' with
  // space so decodeURIComponent functions properly.
  var decodeURI = function decodeURI(s) {
      return decodeURIComponent(s.replace(/\+/g, ' '));
  };
  var match;
  while (match = r.exec(window.location.search.substring(1)))
    this[decodeURI(match[1])] = decodeURI(match[2]);
  this.useMSE = this.useMSE == '1' || this.useMSE == 'true';
  this.playTwice = this.playTwice == '1' || this.playTwice == 'true';
  this.policyCheck = this.policyCheck == '1' || this.policyCheck == 'true';
};

TestConfig.updateDocument = function() {
  this.loadQueryParams();
  Utils.addOptions(KEYSYSTEM_ELEMENT_ID, KEY_SYSTEMS);
  Utils.addOptions(MEDIA_TYPE_ELEMENT_ID, MEDIA_TYPES);

  document.getElementById(MEDIA_FILE_ELEMENT_ID).value =
      this.mediaFile || DEFAULT_MEDIA_FILE;

  document.getElementById(LICENSE_SERVER_ELEMENT_ID).value =
      this.licenseServerURL || DEFAULT_LICENSE_SERVER;

  if (this.keySystem)
    Utils.ensureOptionInList(KEYSYSTEM_ELEMENT_ID, this.keySystem);
  if (this.mediaType)
    Utils.ensureOptionInList(MEDIA_TYPE_ELEMENT_ID, this.mediaType);
  document.getElementById(USE_MSE_ELEMENT_ID).value = this.useMSE;
  document.getElementById(USE_PLAY_TWICE_ELEMENT_ID).value = this.playTwice;
};

TestConfig.init = function() {
  // Reload test configuration from document.
  this.mediaFile = document.getElementById(MEDIA_FILE_ELEMENT_ID).value;
  this.keySystem = document.getElementById(KEYSYSTEM_ELEMENT_ID).value;
  this.mediaType = document.getElementById(MEDIA_TYPE_ELEMENT_ID).value;
  this.useMSE = document.getElementById(USE_MSE_ELEMENT_ID).value == 'true';
  this.playTwice =
      document.getElementById(USE_PLAY_TWICE_ELEMENT_ID).value == 'true';
  this.licenseServerURL =
      document.getElementById(LICENSE_SERVER_ELEMENT_ID).value;
};
