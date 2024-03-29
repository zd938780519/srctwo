// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

window.onload = function() {
  var FILES_APP_ORIGIN = 'chrome-extension://hhaomjibdihmijegdhdafkllkbggdgoj';
  var messageSource;

  var content = document.getElementById('content');

  window.addEventListener('message', function(event) {
    if (event.origin !== FILES_APP_ORIGIN) {
      console.error('Unknown origin: ' + event.origin);
      return;
    }
    messageSource = event.source;
    content.src = event.data;
  });

  document.addEventListener('contextmenu', function(e) {
    e.preventDefault();
    return false;
  });

  document.addEventListener('click', function(e) {
    var data;
    if (e.target === content) {
      data = 'tap-inside';
    } else {
      data = 'tap-outside';
    }

    if (messageSource)
      messageSource.postMessage(data, FILES_APP_ORIGIN);
  });

  // TODO(oka): This is a workaround to fix FOUC problem, where sometimes
  // immature view with smaller window size than outer window is rendered for a
  // moment. Remove this after the root cause is fixed. http://crbug.com/640525
  window.addEventListener('resize', function() {
    // Remove hidden attribute on event of resize to avoid FOUC. The window's
    // initial size is 100 x 100 and it's fit into the outer window size after a
    // moment. Due to Files App's window size constraint, resized window must be
    // larger than 100 x 100. So this event is always invoked.
    content.removeAttribute('hidden');
  });
  // Fallback for the case of webview bug is fixed and above code is not
  // executed.
  setTimeout(function() {
    content.removeAttribute('hidden');
  }, 500);

};
