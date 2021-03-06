// DO NOT EDIT! This test has been generated by tools/gentest.py.
// OffscreenCanvas test in a worker:2d.strokeRect.zero.4
// Description:strokeRect of Nx0 pixels draws a closed line with no caps
// Note:

importScripts("/resources/testharness.js");
importScripts("/common/canvas-tests.js");

var t = async_test("strokeRect of Nx0 pixels draws a closed line with no caps");
t.step(function() {

var offscreenCanvas = new OffscreenCanvas(100, 50);
var ctx = offscreenCanvas.getContext('2d');

ctx.strokeStyle = '#f00';
ctx.lineWidth = 250;
ctx.lineCap = 'round';
ctx.strokeRect(100, 25, 100, 0);
_assertPixel(offscreenCanvas, 50,25, 0,0,0,0, "50,25", "0,0,0,0");

t.done();

});
done();
