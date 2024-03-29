// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/renderer_host/media/in_process_launched_video_capture_device.h"

#include "base/metrics/histogram_macros.h"
#include "content/public/browser/browser_thread.h"

#if defined(ENABLE_SCREEN_CAPTURE) && !defined(OS_ANDROID)
#include "content/browser/media/capture/desktop_capture_device.h"
#if defined(USE_AURA)
#include "content/browser/media/capture/desktop_capture_device_aura.h"
#endif
#endif

#if defined(ENABLE_SCREEN_CAPTURE) && defined(OS_ANDROID)
#include "content/browser/media/capture/screen_capture_device_android.h"
#endif

namespace {

void StopAndReleaseDeviceOnDeviceThread(media::VideoCaptureDevice* device,
                                        base::OnceClosure done_cb) {
  SCOPED_UMA_HISTOGRAM_TIMER("Media.VideoCaptureManager.StopDeviceTime");
  device->StopAndDeAllocate();
  DVLOG(3) << "StopAndReleaseDeviceOnDeviceThread";
  delete device;
  base::ResetAndReturn(&done_cb).Run();
}

}  // anonymous namespace

namespace content {

InProcessLaunchedVideoCaptureDevice::InProcessLaunchedVideoCaptureDevice(
    std::unique_ptr<media::VideoCaptureDevice> device,
    scoped_refptr<base::SingleThreadTaskRunner> device_task_runner)
    : device_(std::move(device)),
      device_task_runner_(std::move(device_task_runner)) {}

InProcessLaunchedVideoCaptureDevice::~InProcessLaunchedVideoCaptureDevice() {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  DCHECK(device_);
  media::VideoCaptureDevice* device_ptr = device_.release();
  device_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(
          &StopAndReleaseDeviceOnDeviceThread, device_ptr,
          base::Bind([](scoped_refptr<base::SingleThreadTaskRunner>) {},
                     device_task_runner_)));
}

void InProcessLaunchedVideoCaptureDevice::GetPhotoState(
    media::VideoCaptureDevice::GetPhotoStateCallback callback) const {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&media::VideoCaptureDevice::GetPhotoState,
                     base::Unretained(device_.get()), base::Passed(&callback)));
}

void InProcessLaunchedVideoCaptureDevice::SetPhotoOptions(
    media::mojom::PhotoSettingsPtr settings,
    media::VideoCaptureDevice::SetPhotoOptionsCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&media::VideoCaptureDevice::SetPhotoOptions,
                     base::Unretained(device_.get()), base::Passed(&settings),
                     base::Passed(&callback)));
}

void InProcessLaunchedVideoCaptureDevice::TakePhoto(
    media::VideoCaptureDevice::TakePhotoCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&media::VideoCaptureDevice::TakePhoto,
                     base::Unretained(device_.get()), base::Passed(&callback)));
}

void InProcessLaunchedVideoCaptureDevice::MaybeSuspendDevice() {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&media::VideoCaptureDevice::MaybeSuspend,
                                base::Unretained(device_.get())));
}

void InProcessLaunchedVideoCaptureDevice::ResumeDevice() {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&media::VideoCaptureDevice::Resume,
                                base::Unretained(device_.get())));
}

void InProcessLaunchedVideoCaptureDevice::RequestRefreshFrame() {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&media::VideoCaptureDevice::RequestRefreshFrame,
                                base::Unretained(device_.get())));
}

void InProcessLaunchedVideoCaptureDevice::SetDesktopCaptureWindowIdAsync(
    gfx::NativeViewId window_id,
    base::OnceClosure done_cb) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Post |device_| to the the device_task_runner_. This is safe since the
  // device is destroyed on the device_task_runner_ and |done_cb|
  // guarantees that |this| stays alive.
  device_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&InProcessLaunchedVideoCaptureDevice::
                                    SetDesktopCaptureWindowIdOnDeviceThread,
                                base::Unretained(this), device_.get(),
                                window_id, base::Passed(&done_cb)));
}

void InProcessLaunchedVideoCaptureDevice::OnUtilizationReport(
    int frame_feedback_id,
    double utilization) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // Unretained() is safe to use here because |device| would be null if it
  // was scheduled for shutdown and destruction, and because this task is
  // guaranteed to run before the task that destroys the |device|.
  device_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&media::VideoCaptureDevice::OnUtilizationReport,
                                base::Unretained(device_.get()),
                                frame_feedback_id, utilization));
}

void InProcessLaunchedVideoCaptureDevice::
    SetDesktopCaptureWindowIdOnDeviceThread(media::VideoCaptureDevice* device,
                                            gfx::NativeViewId window_id,
                                            base::OnceClosure done_cb) {
  DCHECK(device_task_runner_->BelongsToCurrentThread());
#if defined(ENABLE_SCREEN_CAPTURE) && BUILDFLAG(ENABLE_WEBRTC) && \
    !defined(OS_ANDROID)
  DesktopCaptureDevice* desktop_device =
      static_cast<DesktopCaptureDevice*>(device);
  desktop_device->SetNotificationWindowId(window_id);
  VLOG(2) << "Screen capture notification window passed on device thread.";
#endif
  base::ResetAndReturn(&done_cb).Run();
}

}  // namespace content
