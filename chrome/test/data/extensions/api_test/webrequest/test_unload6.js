// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const hostname1 = 'hostname-of-main-frame';
const hostname2 = 'origin-different-from-main-frame';
const hostname3 = 'origin-different-from-other-frames';

// Wait until |num| webRequest.onErrorOccurred sub_frame events have triggered
// and invoke |callback| with the results. We use this instead of the usual
// test framework because a frame may or may not have onBeforeRequest events,
// and at this point we are only interested in seeing whether the details in the
// onErrorOccurred event are valid.
function awaitOnErrorOccurred(num, callback) {
  var callbackDone = chrome.test.callbackAdded();
  var results = [];
  chrome.webRequest.onErrorOccurred.addListener(function listener(details) {
    delete details.requestId;
    delete details.timeStamp;
    details.frameId = normalizeFrameId(details.frameId);
    details.parentFrameId = normalizeFrameId(details.parentFrameId);
    results.push(details);

    if (results.length === num) {
      chrome.webRequest.onErrorOccurred.removeListener(listener);
      callback(results);
      callbackDone();
    }
  }, {
    urls: ['<all_urls>'],
    types: ['sub_frame'],
  });
}

// Generate a deterministic frameId for a given frameId.
function normalizeFrameId(frameId) {
  chrome.test.assertTrue(typeof frameId == 'number');
  if (frameId === -1 || frameId === 0)
    return frameId; // unknown or main frame.
  if (!(frameId in normalizeFrameId.cached))
    normalizeFrameId.cached[frameId] =
      Object.keys(normalizeFrameId.cached).length + 1;
  return normalizeFrameId.cached[frameId];
}
normalizeFrameId.cached = {};

runTests([
  // Insert a frame from the same origin as the main frame, insert another frame
  // from a different origin and immediately remove both.
  function insertMultipleOriginFramesAndImmediatelyRemoveFrames() {
    const mainUrl = getServerURL('empty.html', hostname1);
    const frameUrl1 = getSlowURL(hostname1);
    const frameUrl2 = getSlowURL(hostname2);

    awaitOnErrorOccurred(2, function(results) {
      // The order of the URLs doesn't matter.
      var expectedUrls = [frameUrl1, frameUrl2].sort();
      var actualUrls = results.map(r => r.url).sort();
      chrome.test.assertEq(expectedUrls, actualUrls);
      delete results[0].url;
      delete results[1].url;

      // The main purpose of this check is to see whether the frameId/tabId
      // makes sense.
      chrome.test.assertEq([{
        method: 'GET',
        frameId: 1,
        parentFrameId: 0,
        tabId,
        type: 'sub_frame',
        fromCache: false,
        error: 'net::ERR_ABORTED',
      }, {
        method: 'GET',
        frameId: 2,
        parentFrameId: 0,
        tabId,
        type: 'sub_frame',
        fromCache: false,
        error: 'net::ERR_ABORTED',
      }], results);
    });

    navigateAndWait(mainUrl, function() {
      chrome.tabs.executeScript(tabId, {
        code: `
          var f1 = document.createElement('iframe');
          f1.src = '${frameUrl1}';
          var f2 = document.createElement('iframe');
          f2.src = '${frameUrl2}';

          document.body.appendChild(f1);
          document.body.appendChild(f2);

          f1.remove();
          f2.remove();
        `
      });
    });
  },

  // Insert a frame from yet another origin and immediately remove it.
  // This test re-uses the tab from the previous test.
  function insertCrossOriginFrameAndImmediatelyRemoveFrame() {
    const frameUrl = getSlowURL(hostname3);

    awaitOnErrorOccurred(1, function(results) {
      chrome.test.assertEq([{
        method: 'GET',
        url: frameUrl,
        frameId: 3,
        parentFrameId: 0,
        tabId,
        type: 'sub_frame',
        fromCache: false,
        error: 'net::ERR_ABORTED',
      }], results);
    });

    chrome.tabs.executeScript(tabId, {
      code: `
        var f = document.createElement('iframe');
        f.src = '${frameUrl}';
        document.body.appendChild(f);
        f.remove();
      `
    });
  },
]);
