// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "headless/lib/browser/headless_focus_client.h"

#include "ui/aura/client/focus_change_observer.h"
#include "ui/aura/window.h"

namespace headless {

HeadlessFocusClient::HeadlessFocusClient()
    : focused_window_(NULL), observer_manager_(this) {}

HeadlessFocusClient::~HeadlessFocusClient() {}

void HeadlessFocusClient::AddObserver(
    aura::client::FocusChangeObserver* observer) {
  focus_observers_.AddObserver(observer);
}

void HeadlessFocusClient::RemoveObserver(
    aura::client::FocusChangeObserver* observer) {
  focus_observers_.RemoveObserver(observer);
}

void HeadlessFocusClient::FocusWindow(aura::Window* window) {
  if (window && !window->CanFocus())
    return;

  if (focused_window_)
    observer_manager_.Remove(focused_window_);
  aura::Window* old_focused_window = focused_window_;
  focused_window_ = window;
  if (focused_window_)
    observer_manager_.Add(focused_window_);

  for (aura::client::FocusChangeObserver& observer : focus_observers_)
    observer.OnWindowFocused(focused_window_, old_focused_window);
  aura::client::FocusChangeObserver* observer =
      aura::client::GetFocusChangeObserver(old_focused_window);
  if (observer)
    observer->OnWindowFocused(focused_window_, old_focused_window);
  observer = aura::client::GetFocusChangeObserver(focused_window_);
  if (observer)
    observer->OnWindowFocused(focused_window_, old_focused_window);
}

void HeadlessFocusClient::ResetFocusWithinActiveWindow(aura::Window* window) {
  if (!window->Contains(focused_window_))
    FocusWindow(window);
}

aura::Window* HeadlessFocusClient::GetFocusedWindow() {
  return focused_window_;
}

void HeadlessFocusClient::OnWindowDestroying(aura::Window* window) {
  DCHECK_EQ(window, focused_window_);
  FocusWindow(NULL);
}

}  // namespace headless
