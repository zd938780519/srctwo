// DO NOT EDIT! This test has been generated by tools/gentest.py.
// OffscreenCanvas test in a worker:2d.gradient.object.invalidcolour
// Description:
// Note:

importScripts("/resources/testharness.js");
importScripts("/common/canvas-tests.js");

var t = async_test("");
t.step(function() {

var offscreenCanvas = new OffscreenCanvas(100, 50);
var ctx = offscreenCanvas.getContext('2d');

var g = ctx.createLinearGradient(0, 0, 100, 0);
assert_throws("SYNTAX_ERR", function() { g.addColorStop(0, ""); });
assert_throws("SYNTAX_ERR", function() { g.addColorStop(0, 'null'); });
assert_throws("SYNTAX_ERR", function() { g.addColorStop(0, 'undefined'); });
assert_throws("SYNTAX_ERR", function() { g.addColorStop(0, null); });
assert_throws("SYNTAX_ERR", function() { g.addColorStop(0, undefined); });

t.done();

});
done();
