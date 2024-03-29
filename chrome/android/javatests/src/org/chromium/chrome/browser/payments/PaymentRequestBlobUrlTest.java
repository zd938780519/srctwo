// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.payments;

import android.support.test.filters.MediumTest;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import org.chromium.base.test.util.CommandLineFlags;
import org.chromium.base.test.util.Feature;
import org.chromium.chrome.browser.ChromeSwitches;
import org.chromium.chrome.test.ChromeActivityTestRule;
import org.chromium.chrome.test.ChromeJUnit4ClassRunner;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeoutException;

/** Web payments test for blob URL.  */
@RunWith(ChromeJUnit4ClassRunner.class)
@CommandLineFlags.Add({
        ChromeSwitches.DISABLE_FIRST_RUN_EXPERIENCE,
        ChromeActivityTestRule.DISABLE_NETWORK_PREDICTION_FLAG,
})
public class PaymentRequestBlobUrlTest {
    @Rule
    public PaymentRequestTestRule mPaymentRequestTestRule =
            new PaymentRequestTestRule("payment_request_blob_url_test.html");

    @Test
    @MediumTest
    @Feature({"Payments"})
    public void test() throws InterruptedException, ExecutionException, TimeoutException {
        mPaymentRequestTestRule.openPageAndClickNode("buy");
        mPaymentRequestTestRule.assertWaitForPageScaleFactorMatch(2);
        mPaymentRequestTestRule.expectResultContains(
                new String[] {"SecurityError: Failed to construct 'PaymentRequest': "
                        + "Must be in a secure context"});
    }
}
