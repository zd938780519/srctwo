// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/android/infobars/data_reduction_promo_infobar.h"

#include <utility>

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "chrome/browser/android/resource_mapper.h"
#include "chrome/browser/net/spdyproxy/data_reduction_promo_infobar_delegate_android.h"
#include "content/public/browser/web_contents.h"

// DataReductionPromoInfoBar --------------------------------------------------

DataReductionPromoInfoBar::DataReductionPromoInfoBar(
    std::unique_ptr<DataReductionPromoInfoBarDelegateAndroid> delegate)
    : ConfirmInfoBar(std::move(delegate)) {}

DataReductionPromoInfoBar::~DataReductionPromoInfoBar() {
}

base::android::ScopedJavaLocalRef<jobject>
DataReductionPromoInfoBar::CreateRenderInfoBar(JNIEnv* env) {
  return GetDelegate()->CreateRenderInfoBar(env);
}

DataReductionPromoInfoBarDelegateAndroid*
DataReductionPromoInfoBar::GetDelegate() {
  return static_cast<DataReductionPromoInfoBarDelegateAndroid*>(delegate());
}

// DataReductionPromoInfoBarDelegate ------------------------------------------

// static
std::unique_ptr<infobars::InfoBar>
DataReductionPromoInfoBarDelegateAndroid::CreateInfoBar(
    infobars::InfoBarManager* infobar_manager,
    std::unique_ptr<DataReductionPromoInfoBarDelegateAndroid> delegate) {
  return base::MakeUnique<DataReductionPromoInfoBar>(std::move(delegate));
}
