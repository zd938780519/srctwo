// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/chromeos/mock_camera_module.h"

#include <memory>
#include <utility>

namespace media {
namespace unittest_internal {

MockCameraModule::MockCameraModule()
    : mock_module_thread_("MockModuleThread"), binding_(this) {
  mock_module_thread_.Start();
}

MockCameraModule::~MockCameraModule() {
  mock_module_thread_.task_runner()->PostTask(
      FROM_HERE, base::Bind(&MockCameraModule::CloseBindingOnThread,
                            base::Unretained(this)));
  mock_module_thread_.Stop();
}

void MockCameraModule::OpenDevice(
    int32_t camera_id,
    arc::mojom::Camera3DeviceOpsRequest device_ops_request,
    OpenDeviceCallback callback) {
  DoOpenDevice(camera_id, device_ops_request, callback);
}

void MockCameraModule::GetNumberOfCameras(GetNumberOfCamerasCallback callback) {
  DoGetNumberOfCameras(callback);
}

void MockCameraModule::GetCameraInfo(int32_t camera_id,
                                     GetCameraInfoCallback callback) {
  DoGetCameraInfo(camera_id, callback);
}

void MockCameraModule::SetCallbacks(
    arc::mojom::CameraModuleCallbacksPtr callbacks,
    SetCallbacksCallback callback) {
  DoSetCallbacks(callbacks, callback);
  callbacks_ = std::move(callbacks);
  std::move(callback).Run(0);
}

arc::mojom::CameraModulePtrInfo MockCameraModule::GetInterfacePtrInfo() {
  base::WaitableEvent done(base::WaitableEvent::ResetPolicy::MANUAL,
                           base::WaitableEvent::InitialState::NOT_SIGNALED);
  arc::mojom::CameraModulePtrInfo ptr_info;
  mock_module_thread_.task_runner()->PostTask(
      FROM_HERE,
      base::Bind(&MockCameraModule::BindOnThread, base::Unretained(this),
                 base::Unretained(&done), base::Unretained(&ptr_info)));
  done.Wait();
  return ptr_info;
}

void MockCameraModule::CloseBindingOnThread() {
  if (binding_.is_bound()) {
    binding_.Close();
  }
}

void MockCameraModule::BindOnThread(base::WaitableEvent* done,
                                    arc::mojom::CameraModulePtrInfo* ptr_info) {
  arc::mojom::CameraModulePtr camera_module_ptr;
  arc::mojom::CameraModuleRequest camera_module_request =
      mojo::MakeRequest(&camera_module_ptr);
  binding_.Bind(std::move(camera_module_request));
  *ptr_info = camera_module_ptr.PassInterface();
  done->Signal();
}

}  // namespace unittest_internal
}  // namespace media
