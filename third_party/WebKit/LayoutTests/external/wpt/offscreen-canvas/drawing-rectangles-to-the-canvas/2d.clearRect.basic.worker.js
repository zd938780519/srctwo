// DO NOT EDIT! This test has been generated by tools/gentest.py.
// OffscreenCanvas test in a worker:2d.clearRect.basic
// Description:clearRect clears to transparent black
// Note:

importScripts("/resources/testharness.js");
importScripts("/common/canvas-tests.js");

var t = async_test("clearRect clears to transparent black");
t.step(function() {

var offscreenCanvas = new OffscreenCanvas(100, 50);
var ctx = offscreenCanvas.getContext('2d');

ctx.fillStyle = '#f00';
ctx.fillRect(0, 0, 100, 50);
ctx.clearRect(0, 0, 100, 50);
_assertPixel(offscreenCanvas, 50,25, 0,0,0,0, "50,25", "0,0,0,0");

t.done();

});
done();
