// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/android/compositor/resources/toolbar_resource.h"

#include "base/memory/ptr_util.h"

namespace android {

// static
ToolbarResource* ToolbarResource::From(ui::Resource* resource) {
  if (!resource)
    return nullptr;

  DCHECK_EQ(Type::TOOLBAR, resource->type());
  return static_cast<ToolbarResource*>(resource);
}

ToolbarResource::ToolbarResource(gfx::Rect toolbar_rect,
                                 gfx::Rect location_bar_content_rect,
                                 int shadow_height)
    : Resource(Type::TOOLBAR),
      toolbar_rect_(toolbar_rect),
      location_bar_content_rect_(location_bar_content_rect),
      shadow_height_(shadow_height) {}

ToolbarResource::~ToolbarResource() = default;

std::unique_ptr<ui::Resource> ToolbarResource::CreateForCopy() {
  return base::MakeUnique<ToolbarResource>(
      toolbar_rect_, location_bar_content_rect_, shadow_height_);
}

}  // namespace android
