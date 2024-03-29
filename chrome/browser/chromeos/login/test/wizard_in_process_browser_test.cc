// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/login/test/wizard_in_process_browser_test.h"

#include "base/command_line.h"
#include "base/location.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/single_thread_task_runner.h"
#include "base/threading/thread_task_runner_handle.h"
#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/chromeos/login/login_wizard.h"
#include "chrome/browser/chromeos/login/ui/login_display_host.h"
#include "chrome/browser/chromeos/login/wizard_controller.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/common/chrome_switches.h"
#include "chromeos/chromeos_switches.h"
#include "content/public/browser/notification_service.h"

namespace chromeos {

WizardInProcessBrowserTest::WizardInProcessBrowserTest(OobeScreen screen)
    : screen_(screen) {}

void WizardInProcessBrowserTest::SetUp() {
  WizardController::SetZeroDelays();
  InProcessBrowserTest::SetUp();
}

void WizardInProcessBrowserTest::SetUpCommandLine(
    base::CommandLine* command_line) {
  command_line->AppendSwitch(::switches::kNoStartupWindow);
  command_line->AppendSwitch(switches::kLoginManager);
}

void WizardInProcessBrowserTest::SetUpOnMainThread() {
  SetUpWizard();
  if (screen_ != OobeScreen::SCREEN_UNKNOWN) {
    ShowLoginWizard(screen_);
    host_ = LoginDisplayHost::default_host();
  }
}

void WizardInProcessBrowserTest::TearDownOnMainThread() {
  ASSERT_TRUE(base::MessageLoopForUI::IsCurrent());

  // LoginDisplayHost owns controllers and all windows.
  base::ThreadTaskRunnerHandle::Get()->DeleteSoon(FROM_HERE, host_);
  base::RunLoop().RunUntilIdle();
}

}  // namespace chromeos
