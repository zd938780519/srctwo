/* Copyright (c) 2013 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. */

#include "nacl_io/kernel_intercept.h"
#include "nacl_io/kernel_wrap.h"

int lchown(const char* path, uid_t owner, gid_t group) {
  return ki_lchown(path, owner, group);
}
