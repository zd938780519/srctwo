// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.components.background_task_scheduler;

import android.content.Context;

/** Dummy implementation of a background task used for testing. */
class TestBackgroundTask implements BackgroundTask {
    private static int sRescheduleCalls = 0;

    public TestBackgroundTask() {}

    @Override
    public boolean onStartTask(
            Context context, TaskParameters taskParameters, TaskFinishedCallback callback) {
        return false;
    }

    @Override
    public boolean onStopTask(Context context, TaskParameters taskParameters) {
        return false;
    }

    @Override
    public void reschedule(Context context) {
        sRescheduleCalls++;
    }

    public static int getRescheduleCalls() {
        return sRescheduleCalls;
    }

    public static void reset() {
        sRescheduleCalls = 0;
    }
}
