// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file was generated by:
//   ./tools/json_schema_compiler/compiler.py.

/** @fileoverview Interface for inputMethodPrivate that can be overriden. */

assertNotReached('Interface file for Closure Compiler should not be executed.');

/** @interface */
function InputMethodPrivate() {}

InputMethodPrivate.prototype = {
  /**
   * Gets configurations for input methods.
   * @param {function({
   *   isPhysicalKeyboardAutocorrectEnabled: boolean,
   *   isImeMenuActivated: boolean
   * }):void} callback Callback which is called with the config object.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-getInputMethodConfig
   */
  getInputMethodConfig: assertNotReached,

  /**
   * Gets all whitelisted input methods.
   * @param {function(!Array<{
   *   id: string,
   *   name: string,
   *   indicator: string
   * }>):void} callback Callback which is called with the input method objects.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-getInputMethods
   */
  getInputMethods: assertNotReached,

  /**
   * Gets the current input method.
   * @param {function(string):void} callback Callback which is called with the
   *     current input method.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-getCurrentInputMethod
   */
  getCurrentInputMethod: assertNotReached,

  /**
   * Sets the current input method.
   * @param {string} inputMethodId The input method ID to be set as current
   *     input method.
   * @param {function():void=} callback Callback which is called once the
   *     current input method is set. If unsuccessful $(ref:runtime.lastError)
   *     is set.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-setCurrentInputMethod
   */
  setCurrentInputMethod: assertNotReached,

  /**
   * Fetches a list of all the words currently in the dictionary.
   * @param {function(!Array<string>):void} callback Callback which is called
   *     once the list of dictionary words are ready.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-fetchAllDictionaryWords
   */
  fetchAllDictionaryWords: assertNotReached,

  /**
   * Adds a single word to be stored in the dictionary.
   * @param {string} word A new word to add to the dictionary.
   * @param {function():void=} callback Callback which is called once the word
   *     is added. If unsuccessful $(ref:runtime.lastError) is set.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-addWordToDictionary
   */
  addWordToDictionary: assertNotReached,

  /**
   * Gets whether the encrypt sync is enabled.
   * @param {function(boolean):void=} callback Callback which is called to
   *     provide the result.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-getEncryptSyncEnabled
   */
  getEncryptSyncEnabled: assertNotReached,

  /**
   * Sets the XKB layout for the given input method.
   * @param {string} xkb_name The XKB layout name.
   * @param {function():void=} callback Callback which is called when the layout
   *     is set.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-setXkbLayout
   */
  setXkbLayout: assertNotReached,

  /**
   * Fires the input.ime.onMenuItemActivated event.
   * @param {string} engineID ID of the engine to use.
   * @param {string} name Name of the MenuItem which was activated
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-notifyImeMenuItemActivated
   */
  notifyImeMenuItemActivated: assertNotReached,

  /**
   * Shows the input view window. If the input view window is already shown,
   * this function will do nothing.
   * @param {function():void=} callback Called when the operation completes.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-showInputView
   */
  showInputView: assertNotReached,

  /**
   * Opens the options page for the input method extension. If the input method
   * does not have options, this function will do nothing.
   * @param {string} inputMethodId ID of the input method to open options for.
   * @see https://developer.chrome.com/extensions/inputMethodPrivate#method-openOptionsPage
   */
  openOptionsPage: assertNotReached,
};

/**
 * Fired when the input method is changed.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onChanged
 */
InputMethodPrivate.prototype.onChanged;

/**
 * Fired when the composition bounds or cursor bounds are changed.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onCompositionBoundsChanged
 */
InputMethodPrivate.prototype.onCompositionBoundsChanged;

/**
 * Fired when the custom spelling dictionary is loaded.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onDictionaryLoaded
 */
InputMethodPrivate.prototype.onDictionaryLoaded;

/**
 * Fired when words are added or removed from the custom spelling dictionary.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onDictionaryChanged
 */
InputMethodPrivate.prototype.onDictionaryChanged;

/**
 * Fired when the IME menu is activated or deactivated.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onImeMenuActivationChanged
 */
InputMethodPrivate.prototype.onImeMenuActivationChanged;

/**
 * Fired when the input method or the list of active input method IDs is
 * changed.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onImeMenuListChanged
 */
InputMethodPrivate.prototype.onImeMenuListChanged;

/**
 * Fired when the input.ime.setMenuItems or input.ime.updateMenuItems API is
 * called.
 * @type {!ChromeEvent}
 * @see https://developer.chrome.com/extensions/inputMethodPrivate#event-onImeMenuItemsChanged
 */
InputMethodPrivate.prototype.onImeMenuItemsChanged;
