// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_ELF_HOOK_UTIL_TEST_HOOK_UTIL_TEST_DLL_H_
#define CHROME_ELF_HOOK_UTIL_TEST_HOOK_UTIL_TEST_DLL_H_

#include <windows.h>

extern "C" __declspec(dllexport) void ExportedApi();

extern "C" __declspec(dllexport) int ExportedApiCallCount();

#endif  // CHROME_ELF_HOOK_UTIL_TEST_HOOK_UTIL_TEST_DLL_H_
