// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.omaha;

import android.content.Context;
import android.content.SharedPreferences;

import org.chromium.base.ThreadUtils;
import org.chromium.base.VisibleForTesting;

/**
 * Grabs the URL that points to the Android Market page for Chrome.
 * This incurs I/O, so don't use it from the main thread.
 */
public class MarketURLGetter {

    private static final class LazyHolder {
        private static final MarketURLGetter INSTANCE = new MarketURLGetter();
    }

    /** See {@link #getMarketUrl(Context)} */
    static String getMarketUrl(Context context) {
        assert !ThreadUtils.runningOnUiThread();
        MarketURLGetter instance =
                sInstanceForTests == null ? LazyHolder.INSTANCE : sInstanceForTests;
        return instance.getMarketUrlInternal(context);
    }

    @VisibleForTesting
    static void setInstanceForTests(MarketURLGetter getter) {
        sInstanceForTests = getter;
    }

    private static MarketURLGetter sInstanceForTests;

    protected MarketURLGetter() { }

    /** Returns the Play Store URL that points to Chrome. */
    protected String getMarketUrlInternal(Context context) {
        assert !ThreadUtils.runningOnUiThread();
        SharedPreferences prefs = OmahaBase.getSharedPreferences(context);
        return prefs.getString(OmahaBase.PREF_MARKET_URL, "");
    }
}
