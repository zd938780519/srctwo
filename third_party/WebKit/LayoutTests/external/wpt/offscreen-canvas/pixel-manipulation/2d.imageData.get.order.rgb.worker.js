// DO NOT EDIT! This test has been generated by tools/gentest.py.
// OffscreenCanvas test in a worker:2d.imageData.get.order.rgb
// Description:getImageData() returns R then G then B
// Note:

importScripts("/resources/testharness.js");
importScripts("/common/canvas-tests.js");

var t = async_test("getImageData() returns R then G then B");
t.step(function() {

var offscreenCanvas = new OffscreenCanvas(100, 50);
var ctx = offscreenCanvas.getContext('2d');

ctx.fillStyle = '#48c';
ctx.fillRect(0, 0, 100, 50);
var imgdata = ctx.getImageData(0, 0, 10, 10);
_assertSame(imgdata.data[0], 0x44, "imgdata.data[\""+(0)+"\"]", "0x44");
_assertSame(imgdata.data[1], 0x88, "imgdata.data[\""+(1)+"\"]", "0x88");
_assertSame(imgdata.data[2], 0xCC, "imgdata.data[\""+(2)+"\"]", "0xCC");
_assertSame(imgdata.data[3], 255, "imgdata.data[\""+(3)+"\"]", "255");
_assertSame(imgdata.data[4], 0x44, "imgdata.data[\""+(4)+"\"]", "0x44");
_assertSame(imgdata.data[5], 0x88, "imgdata.data[\""+(5)+"\"]", "0x88");
_assertSame(imgdata.data[6], 0xCC, "imgdata.data[\""+(6)+"\"]", "0xCC");
_assertSame(imgdata.data[7], 255, "imgdata.data[\""+(7)+"\"]", "255");

t.done();

});
done();
