// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.components.payments;

import org.chromium.base.annotations.JNINamespace;

import java.util.Locale;

/**
 * Formatter for currency amounts.
 * https://w3c.github.io/browser-payment-api/specs/paymentrequest.html#currencyamount
 */
@JNINamespace("payments")
public class CurrencyFormatter {
    /**
     * Pointer to the native implementation.
     */
    private long mCurrencyFormatterAndroid;

    /**
     * Builds the formatter for the given currency code and the current user locale.
     *
     * @param currencyCode  The currency code. Most commonly, this follows ISO 4217 format: 3 upper
     *                      case ASCII letters. For example, "USD". Format is not restricted. Should
     *                      not be null.
     * @param currencySystem URI specifying the ISO4217 currency code specification. See for
     *          details: https://w3c.github.io/browser-payment-api/#paymentcurrencyamount-dictionary
     *          By default, the value is "urn:iso:std:iso:4217".
     * @param userLocale User's current locale. Should not be null.
     */
    public CurrencyFormatter(String currencyCode, String currencySystem, Locale userLocale) {
        assert currencyCode != null : "currencyCode should not be null";
        assert currencySystem != null : "currencySystem should not be null";
        assert userLocale != null : "userLocale should not be null";

        // Note that this pointer could leak the native object. The called must call destroy() to
        // ensure that the native object is destroyed.
        mCurrencyFormatterAndroid = nativeInitCurrencyFormatterAndroid(
                currencyCode, currencySystem, userLocale.toString());
    }

    /** Will destroy the native object. This class shouldn't be used afterwards. */
    public void destroy() {
        if (mCurrencyFormatterAndroid != 0) {
            nativeDestroy(mCurrencyFormatterAndroid);
            mCurrencyFormatterAndroid = 0;
        }
    }

    /** @return The currency code formatted for display. */
    public String getFormattedCurrencyCode() {
        return nativeGetFormattedCurrencyCode(mCurrencyFormatterAndroid);
    }

    /**
     * Formats the currency string for display. Does not parse the string into a number, because it
     * might be too large. The number is formatted for the current locale and can include a
     * currency symbol (e.g. $) anywhere in the string, but will not contain the currency code
     * (e.g. USD/US). All spaces in the currency are unicode non-breaking space.
     *
     * @param amountValue The number to format. Should be in "^-?[0-9]+(\.[0-9]+)?$" format. Should
     *                    not be null.
     * @return The amount formatted with the specified currency. See description for details.
     */
    public String format(String amountValue) {
        assert amountValue != null : "amountValue should not be null";

        return nativeFormat(mCurrencyFormatterAndroid, amountValue);
    }

    private native long nativeInitCurrencyFormatterAndroid(
            String currencyCode, String currencySystem, String localeName);
    private native void nativeDestroy(long nativeCurrencyFormatterAndroid);
    private native String nativeFormat(long nativeCurrencyFormatterAndroid, String amountValue);
    private native String nativeGetFormattedCurrencyCode(long nativeCurrencyFormatterAndroid);
}
