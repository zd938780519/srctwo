importAutomationScript('/pointerevents/pointerevent_common_input.js');

function inject_input() {
  return mouseRequestPointerLockAndCaptureInTarget('#div1');
}
