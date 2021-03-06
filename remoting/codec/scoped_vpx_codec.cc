// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "remoting/codec/scoped_vpx_codec.h"

#include "base/logging.h"

extern "C" {
#define VPX_CODEC_DISABLE_COMPAT 1
#include "third_party/libvpx/source/libvpx/vpx/vpx_codec.h"
}
namespace remoting {

void VpxCodecDeleter::operator()(vpx_codec_ctx_t* codec) {
  if (codec) {
    vpx_codec_err_t ret = vpx_codec_destroy(codec);
    CHECK_EQ(ret, VPX_CODEC_OK) << "Failed to destroy codec";
    delete codec;
  }
}

}  // namespace remoting
