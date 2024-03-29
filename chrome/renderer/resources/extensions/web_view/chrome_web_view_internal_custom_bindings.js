// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var binding = apiBridge ||
              require('binding').Binding.create('chromeWebViewInternal');
var contextMenusHandlers = require('contextMenusHandlers');

binding.registerCustomHook(function(bindingsAPI) {
  var apiFunctions = bindingsAPI.apiFunctions;

  var handlers = contextMenusHandlers.create(true /* isWebview */);

  apiFunctions.setHandleRequest(
      'contextMenusCreate', handlers.requestHandlers.create);

  apiFunctions.setHandleRequest(
      'contextMenusUpdate', handlers.requestHandlers.update);

  apiFunctions.setHandleRequest(
      'contextMenusRemove', handlers.requestHandlers.remove);

  apiFunctions.setHandleRequest(
      'contextMenusRemoveAll', handlers.requestHandlers.removeAll);
});

if (!apiBridge)
  exports.$set('ChromeWebView', binding.generate());
