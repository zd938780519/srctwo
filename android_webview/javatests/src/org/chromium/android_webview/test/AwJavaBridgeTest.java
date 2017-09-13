// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.android_webview.test;

import android.support.test.InstrumentationRegistry;
import android.support.test.filters.SmallTest;
import android.webkit.JavascriptInterface;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import org.chromium.android_webview.AwContents;
import org.chromium.base.annotations.SuppressFBWarnings;
import org.chromium.base.test.util.Feature;

/**
 * Test suite for the WebView specific JavaBridge features.
 */
@RunWith(AwJUnit4ClassRunner.class)
public class AwJavaBridgeTest {
    @Rule
    public AwActivityTestRule mActivityTestRule = new AwActivityTestRule();

    private TestAwContentsClient mContentsClient = new TestAwContentsClient();
    private AwTestContainerView mTestContainerView;

    @Before
    public void setUp() throws Exception {
        mTestContainerView = mActivityTestRule.createAwTestContainerViewOnMainSync(mContentsClient);
    }

    @Test
    @SmallTest
    @Feature({"AndroidWebView", "Android-JavaBridge"})
    public void testDestroyFromJavaObject() throws Throwable {
        final String html = "<html>Hello World</html>";
        final TestAwContentsClient client2 = new TestAwContentsClient();
        final AwTestContainerView view2 =
                mActivityTestRule.createAwTestContainerViewOnMainSync(client2);
        final AwContents awContents = mTestContainerView.getAwContents();

        @SuppressFBWarnings("UMAC_UNCALLABLE_METHOD_OF_ANONYMOUS_CLASS")
        class Test {
            @JavascriptInterface
            public void destroy() {
                try {
                    InstrumentationRegistry.getInstrumentation().runOnMainSync(
                            () -> awContents.destroy());
                    // Destroying one AwContents from within the JS callback should still
                    // leave others functioning. Note that we must do this asynchronously,
                    // as Blink thread is currently blocked waiting for this method to finish.
                    mActivityTestRule.loadDataAsync(
                            view2.getAwContents(), html, "text/html", false);
                } catch (Throwable t) {
                    throw new RuntimeException(t);
                }
            }
        }

        mActivityTestRule.enableJavaScriptOnUiThread(awContents);
        InstrumentationRegistry.getInstrumentation().runOnMainSync(
                () -> awContents.addJavascriptInterface(new Test(), "test"));

        mActivityTestRule.loadDataSync(
                awContents, mContentsClient.getOnPageFinishedHelper(), html, "text/html", false);

        // Ensure the JS interface object is there, and invoke the test method.
        Assert.assertEquals("\"function\"",
                mActivityTestRule.executeJavaScriptAndWaitForResult(
                        awContents, mContentsClient, "typeof test.destroy"));
        int currentCallCount = client2.getOnPageFinishedHelper().getCallCount();
        awContents.evaluateJavaScriptForTests("test.destroy()", null);

        client2.getOnPageFinishedHelper().waitForCallback(currentCallCount);
    }

    @Test
    @SmallTest
    @Feature({"AndroidWebView", "Android-JavaBridge"})
    public void testTwoWebViewsCreatedSimultaneously() throws Throwable {
        final AwContents awContents1 = mTestContainerView.getAwContents();
        final TestAwContentsClient client2 = new TestAwContentsClient();
        final AwTestContainerView view2 =
                mActivityTestRule.createAwTestContainerViewOnMainSync(client2);
        final AwContents awContents2 = view2.getAwContents();

        mActivityTestRule.enableJavaScriptOnUiThread(awContents1);
        mActivityTestRule.enableJavaScriptOnUiThread(awContents2);

        class Test {
            Test(int value) {
                mValue = value;
            }
            @SuppressFBWarnings("UMAC_UNCALLABLE_METHOD_OF_ANONYMOUS_CLASS")
            @JavascriptInterface
            public int getValue() {
                return mValue;
            }
            private int mValue;
        }

        InstrumentationRegistry.getInstrumentation().runOnMainSync(() -> {
            awContents1.addJavascriptInterface(new Test(1), "test");
            awContents2.addJavascriptInterface(new Test(2), "test");
        });
        final String html = "<html>Hello World</html>";
        mActivityTestRule.loadDataSync(
                awContents1, mContentsClient.getOnPageFinishedHelper(), html, "text/html", false);
        mActivityTestRule.loadDataSync(
                awContents2, client2.getOnPageFinishedHelper(), html, "text/html", false);

        Assert.assertEquals("1",
                mActivityTestRule.executeJavaScriptAndWaitForResult(
                        awContents1, mContentsClient, "test.getValue()"));
        Assert.assertEquals("2",
                mActivityTestRule.executeJavaScriptAndWaitForResult(
                        awContents2, client2, "test.getValue()"));
    }

    @Test
    @SmallTest
    @Feature({"AndroidWebView", "Android-JavaBridge"})
    public void testTwoWebViewsSecondCreatedAfterLoadingInFirst() throws Throwable {
        final AwContents awContents1 = mTestContainerView.getAwContents();
        mActivityTestRule.enableJavaScriptOnUiThread(awContents1);

        class Test {
            Test(int value) {
                mValue = value;
            }
            @SuppressFBWarnings("UMAC_UNCALLABLE_METHOD_OF_ANONYMOUS_CLASS")
            @JavascriptInterface
            public int getValue() {
                return mValue;
            }
            private int mValue;
        }

        InstrumentationRegistry.getInstrumentation().runOnMainSync(
                () -> awContents1.addJavascriptInterface(new Test(1), "test"));
        final String html = "<html>Hello World</html>";
        mActivityTestRule.loadDataSync(
                awContents1, mContentsClient.getOnPageFinishedHelper(), html, "text/html", false);
        Assert.assertEquals("1",
                mActivityTestRule.executeJavaScriptAndWaitForResult(
                        awContents1, mContentsClient, "test.getValue()"));

        final TestAwContentsClient client2 = new TestAwContentsClient();
        final AwTestContainerView view2 =
                mActivityTestRule.createAwTestContainerViewOnMainSync(client2);
        final AwContents awContents2 = view2.getAwContents();
        mActivityTestRule.enableJavaScriptOnUiThread(awContents2);

        InstrumentationRegistry.getInstrumentation().runOnMainSync(
                () -> awContents2.addJavascriptInterface(new Test(2), "test"));
        mActivityTestRule.loadDataSync(
                awContents2, client2.getOnPageFinishedHelper(), html, "text/html", false);

        Assert.assertEquals("1",
                mActivityTestRule.executeJavaScriptAndWaitForResult(
                        awContents1, mContentsClient, "test.getValue()"));
        Assert.assertEquals("2",
                mActivityTestRule.executeJavaScriptAndWaitForResult(
                        awContents2, client2, "test.getValue()"));
    }
}