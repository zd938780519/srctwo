// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.notifications;

import android.annotation.TargetApi;
import android.app.job.JobParameters;
import android.app.job.JobService;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.PersistableBundle;

import org.chromium.base.ThreadUtils;

/**
 * Processes jobs scheduled when user actions are issued on web notifications.
 * We use this instead of starting the NotificationService on N+.
 */
@TargetApi(Build.VERSION_CODES.N)
public class NotificationJobService extends JobService {
    static PersistableBundle getJobExtrasFromIntent(Intent intent) {
        PersistableBundle bundle = new PersistableBundle();
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_ID,
                intent.getStringExtra(NotificationConstants.EXTRA_NOTIFICATION_ID));
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_INFO_ORIGIN,
                intent.getStringExtra(NotificationConstants.EXTRA_NOTIFICATION_INFO_ORIGIN));
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_INFO_PROFILE_ID,
                intent.getStringExtra(NotificationConstants.EXTRA_NOTIFICATION_INFO_PROFILE_ID));
        bundle.putBoolean(NotificationConstants.EXTRA_NOTIFICATION_INFO_PROFILE_INCOGNITO,
                intent.getBooleanExtra(
                        NotificationConstants.EXTRA_NOTIFICATION_INFO_PROFILE_INCOGNITO, false));
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_INFO_TAG,
                intent.getStringExtra(NotificationConstants.EXTRA_NOTIFICATION_INFO_TAG));
        bundle.putInt(NotificationConstants.EXTRA_NOTIFICATION_INFO_ACTION_INDEX,
                intent.getIntExtra(NotificationConstants.EXTRA_NOTIFICATION_INFO_ACTION_INDEX, -1));
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_INFO_WEBAPK_PACKAGE,
                intent.getStringExtra(
                        NotificationConstants.EXTRA_NOTIFICATION_INFO_WEBAPK_PACKAGE));
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_ACTION, intent.getAction());
        // Only primitives can be set on a persistable bundle, so extract the raw reply.
        bundle.putString(NotificationConstants.EXTRA_NOTIFICATION_REPLY,
                NotificationPlatformBridge.getNotificationReply(intent));
        return bundle;
    }

    /**
     * Called when a Notification has been interacted with by the user. If we can verify that
     * the Intent has a notification Id, start Chrome (if needed) on the UI thread.
     *
     * We get a wakelock for our process for the duration of this method.
     *
     * @return True if there is more work to be done to handle the job, to signal we would like our
     * wakelock extended until we call {@link #jobFinished}. False if we have finished handling the
     * job.
     */
    @Override
    public boolean onStartJob(final JobParameters params) {
        PersistableBundle extras = params.getExtras();
        if (!extras.containsKey(NotificationConstants.EXTRA_NOTIFICATION_ID)
                || !extras.containsKey(NotificationConstants.EXTRA_NOTIFICATION_INFO_ORIGIN)
                || !extras.containsKey(NotificationConstants.EXTRA_NOTIFICATION_INFO_TAG)) {
            return false;
        }

        Intent intent =
                new Intent(extras.getString(NotificationConstants.EXTRA_NOTIFICATION_ACTION));
        intent.putExtras(new Bundle(extras));

        ThreadUtils.assertOnUiThread();
        NotificationService.dispatchIntentOnUIThread(this, intent);

        // TODO(crbug.com/685197): Return true here and call jobFinished to release the wake
        // lock only after the event has been completely handled by the service worker.
        return false;
    }

    @Override
    public boolean onStopJob(JobParameters params) {
        // As it stands, all our job processing is done synchronously in onStartJob so there is
        // nothing to do here. Even once we include further async processing in our jobs
        // (crbug.com/685197) it may by infeasible to cancel this halfway through.
        // TODO(crbug.com/685197): Check what we can safely do here and update comment.
        return false;
    }
}
